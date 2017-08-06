/* $Id: database.c,v 2.1 2005/07/07 11:40:41 ksb Exp $ */
/* $Revision: 2.1 $ */

#include <flump.h>

/* pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER; 
*/
pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_db()
{
	pthread_mutex_lock(&db_mutex);
	global.database = mysql_init(global.database);
	if(!mysql_real_connect(global.database,
		config_get("db_host"),
		config_get("db_user"),
		config_get("db_pass"),
		config_get("db_name"),
		0,0,0))
	{
		fprintf(stderr,"Error %s connecting to MySQL Database on %s\n",mysql_error(global.database),config_get("db_host"));
		pthread_mutex_unlock(&db_mutex);
		exit(10);
	}
	debug("Connected to database.");
	pthread_mutex_unlock(&db_mutex);
}

void freelist(struct lists_st *lst)
{
	struct lists_st *sweep=NULL;
	struct lists_st *next=NULL;

	for (sweep=lst;sweep;sweep=next) {
		next=sweep->next;
		free(sweep->value);
		free(sweep);
	}
}

struct lists_st *getvalues(int type, char *attr2)
{
	int ernum;
	MYSQL_RES *result=NULL;
	MYSQL_ROW row;
	char temp[TMP_SIZE];
	int rowcount;
	struct lists_st *sweep=NULL;
	struct lists_st *retlist=NULL;
	struct lists_st *tmplist=NULL;
	char *attr=NULL;
	int blah=0;

	if (attr2) {
		attr=MALLOC(strlen(attr2)*3+2,"getvalues:attr");
		attr[0]='\0';
		if (attr2[0]!='\0') {
			blah=mysql_real_escape_string(global.database,attr,attr2, strlen(attr2));
		}
	}	
	else
	{
		return NULL;
	}
	
	pthread_mutex_lock(&db_mutex);
	sprintf(temp,"select objects.id,attributes.value from objects,attributes where attributes.name='%s' and attributes.object=objects.id and objects.type=%d",attr,type);

	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database Select Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		return NULL;
	}
	result = mysql_store_result(global.database);
	rowcount = mysql_num_rows(result);
	if((!result)||(!rowcount))
	{
		debug("Object query returned empty value");
		MYSQL_FREE(result);
		pthread_mutex_unlock(&db_mutex);
		return NULL;
	}
	while ((row = mysql_fetch_row(result))) {
		tmplist=(struct lists_st *)malloc(sizeof(struct lists_st));
		tmplist->next=NULL;
		tmplist->oid=atol(row[0]);
		tmplist->value=(char *)malloc(strlen(row[1])+4);
		strcpy(tmplist->value,row[1]);
		if (!retlist) {
			retlist=tmplist;
		}
		else
		{
			for (sweep=retlist;sweep->next;sweep=sweep->next);
			sweep->next=tmplist;
		}
	}
	
	MYSQL_FREE(result);
	pthread_mutex_unlock(&db_mutex);
	return retlist;
}

char *db_get_obj_name(unsigned long id)
{
	int ernum;
	MYSQL_RES *result=NULL;
	MYSQL_ROW row;
	char *ret=NULL;
	char temp[TMP_SIZE];
	int rowcount;

	pthread_mutex_lock(&db_mutex);
	sprintf(temp,"SELECT name from objects where id=%ld",id);

	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database Select Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		return NULL;
	}
	result = mysql_store_result(global.database);
	rowcount = mysql_num_rows(result);
	if((!result)||(!rowcount))
	{
		debug("Object query returned empty value");
		MYSQL_FREE(result);
		pthread_mutex_unlock(&db_mutex);
		return NULL;
	}
	row = mysql_fetch_row(result);
	ret = MALLOC(strlen(row[0])+4,"db_get_objid:value");
	strcpy(ret,row[0]);
	
	MYSQL_FREE(result);
	pthread_mutex_unlock(&db_mutex);
	return ret;
}

/* removes an object from the db
 * also removes all of it's attributes
 */
void db_delete_obj(unsigned long id)
{
	int ernum;
	char temp[TMP_SIZE];
	pthread_mutex_lock(&db_mutex);

	sprintf(temp,"DELETE FROM %s WHERE id=%lu",
		config_get("db_objects"),id);
		
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database delete Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		return;
	}	

	delete_all_attributes(id);
	pthread_mutex_unlock(&db_mutex);
}

/* get the object id from name and type
// done as a pointer cos 0 could be a valid id
// returns null for no such object
*/
unsigned long *db_get_objid(char *name, unsigned long type)
{
	int ernum;
	MYSQL_RES *result=NULL;
	MYSQL_ROW row;
	unsigned long *value=NULL;
	int rowcount;
	char temp[TMP_SIZE];
	pthread_mutex_lock(&db_mutex);

	sprintf(temp,"SELECT id FROM %s WHERE name='%s' AND type=%lu",
			config_get("db_objects"), name, type);

	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database Select Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		return NULL;
	}
	result = mysql_store_result(global.database);
	rowcount = mysql_num_rows(result);
	if((!result)||(!rowcount))
	{
		debug("Object query returned empty value");
		MYSQL_FREE(result);
		pthread_mutex_unlock(&db_mutex);
		return NULL;
	}
	row = mysql_fetch_row(result);
	value = MALLOC(sizeof(unsigned long),"db_get_objid:value");
	*value=atol(row[0]);
	
	MYSQL_FREE(result);
	pthread_mutex_unlock(&db_mutex);
	return value;

}

/* Creates a new object of name and type and returns
// the new objects id
// returns NULL on failure (hmmm)
// does NOT check to see if the object already exists so
// this will fuck with your data if you are not carefull
*/
unsigned long *db_set_newobj(char *name, unsigned long type)
{
	int ernum;
	unsigned long *value=NULL;
	char temp[TMP_SIZE];
	pthread_mutex_lock(&db_mutex);

	sprintf(temp,"INSERT INTO %s (name,type) VALUES ('%s',%lu)",
			config_get("db_objects"), name, type);


	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database insert Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		return NULL;
	}
	value = MALLOC(sizeof(unsigned long),"db_set_newobj:value");
	*value = mysql_insert_id(global.database);
		pthread_mutex_unlock(&db_mutex);
	return value;
}

/* USE get_attribute instead for cache control
** using this directly will break stuff
*/
char *db_get_attribute(unsigned long object, char *name)
{
	int ernum;
	MYSQL_RES *result=NULL;
	MYSQL_ROW row;
	unsigned long *lengths=NULL;
	char *value=NULL;
	int rowcount;
	char temp[TMP_SIZE];
	pthread_mutex_lock(&db_mutex);

	sprintf(temp,"SELECT value FROM %s WHERE object=%lu AND name='%s'",
			config_get("db_attributes"), object, name);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database Select Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		return (char *)0;
	}
	result = mysql_store_result(global.database);
	rowcount = mysql_num_rows(result);
	if((!result)||(!rowcount))
	{
		debug("Attribute query returned empty value");
		MYSQL_FREE(result);
		pthread_mutex_unlock(&db_mutex);
		return (char *)0;
	}

	row = mysql_fetch_row(result);
	lengths = mysql_fetch_lengths(result);

	value = MALLOC(lengths[0]+2,"db_get_attribute:value");
	strcpy(value,row[0]);
	
	MYSQL_FREE(result);
	pthread_mutex_unlock(&db_mutex);
	return value;
}	

/* USE set_attribute instead for cache control
** using this directly will break stuff
*/
void db_set_attribute(unsigned long object, char *name, char *value2)
{
	int ernum;
	MYSQL_RES *result=NULL;
	int rows;
	char temp[TMP_SIZE];
	char *value=NULL;
	unsigned long blah=0;
	pthread_mutex_lock(&db_mutex);

	if (value2) {
		value=MALLOC(strlen(value2)*3+2,"db_set_attribute:value");
		value[0]='\0';
		if (value2[0]!='\0') {
			blah=mysql_real_escape_string(global.database,value,value2, strlen(value2));
		}
	}	
	
	sprintf(temp,"SELECT value FROM %s WHERE object=%lu AND name='%s'",
			config_get("db_attributes"),object,name);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
		pthread_mutex_unlock(&db_mutex);
		FREE(value);
		return;
	}
        result = mysql_store_result(global.database);
        if(!result)
        {
                debug("Attribute query returned empty value");
		pthread_mutex_unlock(&db_mutex);
		FREE(value);
                return;
        }
	rows = mysql_num_rows(result);
	MYSQL_FREE(result);
	if(rows>0)
	{
		sprintf(temp,"UPDATE %s SET value='%s' WHERE object=%lu AND name='%s'",
			config_get("db_attributes"),value?value:"",object,name);
	} else {
		sprintf(temp,"INSERT INTO %s SET value='%s', object=%lu, name='%s'",
			config_get("db_attributes"),value?value:"",object,name);
	}
	ernum = mysql_query(global.database,temp);
	FREE(value);
	pthread_mutex_unlock(&db_mutex);
}

void db_delete_attribute(unsigned long object, char *name)
{
	int ernum;
	char temp[TMP_SIZE];
	pthread_mutex_lock(&db_mutex);

	sprintf(temp,"DELETE FROM %s WHERE object=%lu AND name='%s'",
			config_get("db_attributes"),object,name);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database delete failed:");
		debug((char *)mysql_error(global.database));
	}
		pthread_mutex_unlock(&db_mutex);
}

void db_delete_all_attributes(unsigned long object)
{
	int ernum;
	char temp[TMP_SIZE];
	pthread_mutex_lock(&db_mutex);

	sprintf(temp,"DELETE FROM %s WHERE object=%lu",
			config_get("db_attributes"),object);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database delete failed:");
		debug((char *)mysql_error(global.database));
	}
		pthread_mutex_unlock(&db_mutex);
}

