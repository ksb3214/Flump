/* $Id: modules.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

/* This was originally a module but it made no sense as if it were
** unloaded the command to reload wouldn't exist ;)
*/
#include <flump.h>

/* This will hold module info for reloading
*/
struct tmplib {
	char *name;
	struct tmplib *next;
};

char *search_path[] = {
        "modules/user",
        "modules/core",
	"*" };

lib *libs;

int load_dll(char *filename)
{
	lib *sweep=NULL;
	void *handle=NULL;
	int i;
	int found=0;
	char temp[TMP_SIZE];

	handle = dlopen(filename,RTLD_NOW|RTLD_GLOBAL);
	if(!handle)
	{
	fprintf(stderr,"%s\n",dlerror());
		for(i=0; (search_path[i][0]!='*') && (!handle); i++)
		{
			sprintf(temp,"%s/%s",search_path[i],filename);
			fprintf(stderr,"Loading %s\n",temp);
			handle = dlopen(temp,RTLD_NOW|RTLD_GLOBAL);
		}
		if(!handle)
		{
			fprintf(stderr,"%s",dlerror());
			return 0;
		}
		found=1;
	} 
	
	if(!libs)
	{
		libs = MALLOC(sizeof(lib),"load_dll:libs");
		sweep = libs;
	} else {
		for(sweep=libs; sweep->next; sweep=sweep->next);
		sweep->next = MALLOC(sizeof(lib),"load_dll:sweep->next");
		sweep = sweep->next;
	}
	sweep->handle=handle;
	if(found==0)
	{
		sweep->filename = MALLOC(strlen(filename)+2,"load_dll:sweep->filename");
		strcpy(sweep->filename,filename);
	} else {
		sweep->filename = MALLOC(strlen(temp)+2,"load_dll:sweep->filename");
		strcpy(sweep->filename,temp);
	}

	sweep->init = dlsym(sweep->handle,"init");
	sweep->uninit = dlsym(sweep->handle,"uninit");
	sweep->tick = dlsym(sweep->handle,"tick");
	sweep->event = dlsym(sweep->handle,"event");
	sweep->version = dlsym(sweep->handle,"version");
	sweep->next=NULL;
	if(sweep->init) 
	{ 
		if(sweep->init()!=1)
		{
			unload_dll(sweep,1);
			return -1;
		}
	}
	return 1;
}

/* do not pass a null */
int unload_dll(lib *l,int force)
{
	lib *sweep=NULL;
	if(l->uninit)
	{
		if((!l->uninit()) && (!force))
		{
			return 0;
		}
	}
	if(dlclose(l->handle)==-1)
	{
		return 0;
	}
	if (l==libs) {
		libs=l->next;
	}
	else
	{
		for(sweep=libs; sweep; sweep=sweep->next)
		{
			if(sweep->next==l)
			{
				sweep->next = l->next;
			}
		}
	}	
	FREE(l->filename);
	FREE(l);
	return 1;
}

void clear_dlls()
{
	lib *sweep=NULL,*next=NULL;

	for(sweep=libs; sweep; sweep=next)
	{
		next = sweep->next;
		FREE(sweep->filename);
		FREE(sweep);
	}
}

void reload_dll(lib *l)
{
	char *filename=NULL;

	filename = MALLOC(strlen(l->filename)+2,"reload_dll:filename");
	strcpy(filename,l->filename);
	unload_dll(l,0);
	load_dll(filename);
	FREE(filename);
}


void modlist(connection *u,char *arg)
{
	lib *sweep=NULL;
	char temp[TMP_SIZE];

	swrite(NULL,u,"Modules loaded:\n");
	for(sweep=libs; sweep; sweep=sweep->next)
	{
		sprintf(temp,"  %s\n",sweep->filename);
		swrite(NULL,u,temp);
	}
}

void modload(connection *u, char *arg)
{
	lib *sweep=NULL;
	char temp[TMP_SIZE];
	int i;

	if (!arg) {
		swrite(NULL,u,"Soecify a module to load.\n");
		return;
	}	

	for(sweep=libs; sweep; sweep=sweep->next)
	{
		if(!strcasecmp(sweep->filename,arg))
		{
			swrite(NULL,u,"Module already loaded.\n");
			return;
		}
	}

	i=load_dll(arg);
	if(i==0)
	{
		sprintf(temp,"Module load error: %s\n",dlerror());
		swrite(NULL,u,temp);
		return;
	}
	if(i==-1)
	{
		sprintf(temp,"Module load error: module failed to init\n");
		swrite(NULL,u,temp);
		return;
	}

	sprintf(temp,"Module loaded: %s\n",arg);
	swrite(NULL,u,temp);
}

int modunload(connection *u, char *arg)
{
	lib *sweep=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"usage: modunload [path/module.so]\n");
		return 0;
	}

	for(sweep=libs; sweep; sweep=sweep->next)
	{
		if(!strcasecmp(sweep->filename,arg))
		{
			if(!unload_dll(sweep,0))
			{
				swrite(NULL,u,"Module cannot be unloaded.\n");
				return 0;
			}
			sprintf(temp,"Module unloaded: %s\n",arg);
			swrite(NULL,u,temp);
			return 1;
		}
	}
	swrite(NULL,u,"Module not loaded.\n");
	return 2;
}

void modreloadall(connection *u, char *arg)
{
	lib *sweep=NULL;
	char temp[TMP_SIZE];
	struct tmplib *tmplib1=NULL;
	struct tmplib *tmplib2=NULL; /* for stepping */

	for(sweep=libs; sweep; sweep=sweep->next) {
		if (!tmplib1) {
			tmplib2=MALLOC(sizeof(struct tmplib),"modreloadall:tmplib2");
			tmplib1=tmplib2;
		}
		else
		{
			tmplib2->next=MALLOC(sizeof(struct tmplib),"modreloadall:tmplib2->next");
			tmplib2=tmplib2->next;
		}
		tmplib2->name=MALLOC(strlen(sweep->filename)+4,"modreloadall:tmplib2->name");
		strcpy(tmplib2->name,sweep->filename);
		tmplib2->next=NULL;
	}

		
	for(tmplib2=tmplib1; tmplib2; tmplib2=tmplib2->next)
	{
		sprintf(temp,"Attempting to unload: %s\n",tmplib2->name);
		swrite(NULL,u,temp);
		strcpy(temp, tmplib2->name);
		if (modunload(u, temp)) {
			modload(u, temp);
		}	
	}

	for(tmplib2=tmplib1; tmplib2; tmplib2=tmplib1)
	{
		tmplib1=tmplib2->next;
		FREE(tmplib2->name);
		FREE(tmplib2);
	}	
}

unsigned long register_module(char *name)
{
	unsigned long *objid=NULL;
	unsigned long id;

	objid=db_get_objid(name,OB_MODULE);
	if(!objid)
	{
		objid=db_set_newobj(name,OB_MODULE);
		if(!objid)
		{
			return 0;
		}
	}
	id = *objid;
	FREE(objid);
	return id;
}
