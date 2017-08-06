/* $Id: cache.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <flump.h>

cache *attcache;
short maxcache;

pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER; 

void check_cache()
{
	cache *sweep=NULL;
	int count=0;
	pthread_mutex_lock(&cache_mutex);

	for(sweep = attcache; sweep; sweep=sweep->next)
	{
		printf("Block    - %d\n",count);
		printf("  Name   - (%s)\n",sweep->name);
		printf("  Value  - (%s)\n",sweep->value);
		printf("  Object - (%lu)\n",sweep->object);
		count++;
	}
	pthread_mutex_unlock(&cache_mutex);
}

char *get_attribute(unsigned long object, char *name)
{
	cache *sweep=NULL;
	short cachecount=0;
	char *retval=NULL;
	cache *oldest=NULL;
	cache *last=NULL;

	pthread_mutex_lock(&cache_mutex);

	for(sweep = attcache; sweep; sweep=sweep->next)
	{
		if(!strcasecmp(name,sweep->name) && sweep->object==object)
		{
			if (sweep->value) {
				retval=MALLOC((strlen(sweep->value)+2)*sizeof(char),"get_attribute:retval");
				strcpy(retval, sweep->value);
			}
			else
			{
				retval=NULL;
			}	
			pthread_mutex_unlock(&cache_mutex);
			return retval;
		}
	}

	cachecount=0;
	for(sweep = attcache; sweep; sweep=sweep->next)
	{
		cachecount++;
	}

	if(cachecount>=maxcache)
	{
		oldest = attcache;

		for(sweep = attcache; sweep; sweep=sweep->next)
		{
			if(sweep->timestamp<oldest->timestamp)
			{
				oldest=sweep;
			}
		}
		for(sweep = attcache; sweep!=oldest; sweep=sweep->next) {
			last=sweep;
		}
		FREE(oldest->name);
		FREE(oldest->value);
		if (oldest!=attcache) {
			last->next=oldest->next;
		}
		else
		{
			attcache=oldest->next;
		}	
		FREE(oldest);
	}
	if(attcache)
	{
		for(sweep = attcache; sweep->next; sweep=sweep->next);
		sweep->next = (cache *)MALLOC((int)sizeof(cache),"get_attribute:sweep->next");
		sweep = sweep->next;
	} else {
		attcache = (cache *)MALLOC(sizeof(cache),"get_attributei:attcache");
		sweep = attcache;
	}
		
	sweep->object = object;
	sweep->name = MALLOC(strlen(name)+2,"get_attribute:sweep->name"); strcpy(sweep->name,name);
	sweep->value = db_get_attribute(object,name);
	sweep->timestamp = time(0);
	sweep->next=NULL;
	if (sweep->value) {
		retval=MALLOC((strlen(sweep->value)+2)*sizeof(char),"get_attribute:retval");
		strcpy(retval, sweep->value);
	}
	else
	{
		retval=NULL;
	}	
	pthread_mutex_unlock(&cache_mutex);
	return retval;
}
 
void set_attribute(unsigned long object, char *name, char *value) 
{ 
	cache *sweep=NULL;
	cache *last=NULL;
	int updated=0;
	int cachecount=0;

	pthread_mutex_lock(&cache_mutex);

	for(sweep = attcache; sweep; sweep=sweep->next)
	{
		if(!strcasecmp(sweep->name,name) && sweep->object==object)
		{
			FREE(sweep->value);
			sweep->value=MALLOC(strlen(value)+2,"set_attribute:sweep->value");
			strcpy(sweep->value,value);
			updated=1;
		}
	}
	if(!updated)
	{
	        for(sweep = attcache; sweep; sweep=sweep->next)
		{
			cachecount++;
		}

		if(cachecount>=maxcache)
		{
			cache *oldest;

			oldest = attcache;

			for(sweep = attcache; sweep; sweep=sweep->next)
			{
				if(sweep->timestamp<oldest->timestamp)
				{
					oldest=sweep;
				}
			}

			for(sweep = attcache; sweep!=oldest; sweep=sweep->next) 
			{
				last=sweep;
			}
			FREE(oldest->name);
			FREE(oldest->value);
			if (oldest!=attcache) {
				last->next=oldest->next;
			}
			else
			{
				attcache=oldest->next;
			}	
			FREE(oldest);


/*
			FREE(oldest->name);
			FREE(oldest->value);
			sweep->next=oldest->next;
			FREE(oldest);
		*/	
		}
		if(attcache)
		{
			for(sweep = attcache; sweep->next; sweep=sweep->next);
			sweep->next = MALLOC(sizeof(cache),"set_attribute:sweep->next");
			sweep = sweep->next;
		} else {
			attcache = MALLOC(sizeof(cache),"set_attribute:attcache");
			sweep = attcache;
		}

		sweep->object = object;
		sweep->name = MALLOC(strlen(name)+2,"set_attribute:sweep->name"); strcpy(sweep->name,name);
		sweep->value = MALLOC(strlen(value)+2,"set_attribute:sweep->value"); strcpy(sweep->value,value);
		sweep->timestamp = time(0);
		sweep->next=NULL;
	}
	db_set_attribute(object,name,value); 
	pthread_mutex_unlock(&cache_mutex);
}

void cache_clear()
{
	cache *sweep=NULL;
	cache *next=NULL;

	pthread_mutex_lock(&cache_mutex);

	for(sweep=attcache; sweep; sweep=next)
	{
		next=sweep->next;
		FREE(sweep->name);
		FREE(sweep->value);
		FREE(sweep);
	}

	pthread_mutex_unlock(&cache_mutex);
}

void delete_attribute(unsigned long object, char *name)
{
	cache *sweep=NULL;
	cache *last=NULL;
	cache *next=NULL;
	pthread_mutex_lock(&cache_mutex);
	for(sweep=attcache; sweep; sweep=next)
	{
		next=sweep->next;
		if(!strcasecmp(sweep->name,name) && sweep->object==object)
		{
			FREE(sweep->name);
			FREE(sweep->value);
			if (sweep==attcache) {
				attcache=sweep->next;
			}
			else
			{
				last->next=sweep->next;
			}
			/* last=sweep->next; */
			FREE(sweep);
		}
		else
		{
		 	last=sweep;
		}	
	}
	db_delete_attribute(object,name);
	pthread_mutex_unlock(&cache_mutex);
}

void delete_all_attributes(unsigned long object)
{
	cache *sweep=NULL;
	cache *last=NULL;
	cache *next=NULL;
	pthread_mutex_lock(&cache_mutex);
	for(sweep=attcache; sweep; sweep=next)
	{
		next=sweep->next;
		if(sweep->object==object)
		{
			FREE(sweep->name);
			FREE(sweep->value);
			if (sweep==attcache) {
				attcache=sweep->next;
			}
			else
			{
				last->next=sweep->next;
			}
			/* last=sweep->next; */
			FREE(sweep);
		}
		else
		{
		 	last=sweep;
		}	
	}
	db_delete_all_attributes(object);
	pthread_mutex_unlock(&cache_mutex);
}
