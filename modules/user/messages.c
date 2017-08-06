/* $Id: messages.c,v 2.1 2005/07/07 16:27:09 ksb Exp $ */
/* $Revision: 2.1 $ */

/* must be loaded after lists.so */

#include "flump.h"

extern pthread_mutex_t db_mutex;
int motdcount=0;

void smess(connection *u, char *arg)
{
	int ernum;
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE*2];
	unsigned long blah=0;
	char *mes=NULL;
	unsigned long *uid=NULL;
	connection *target=NULL;

	if (!arg) {
		swrite(NULL,u,"smess [user] [message].\n");
		return;
	}	

	mes=strchr(arg,' ');

	if (!mes) {
		swrite(NULL,u,"You forgot the message.\n");
		return;
	}

	*mes='\0';
	mes++;

	uid=db_get_objid(arg,OB_USER);
	if (!uid) {
		swrite(NULL,u,"No such user stored (use full name).\n");
		return;
	}

	if (*uid==u->object) {
		swrite(NULL,u,"Smessing yourself eh, silly!\n");
		FREE(uid);
		return;
	}

	if (strlen(mes)>TMP_SIZE-1) {
		swrite(NULL,u,"Message too long.\n");
		FREE(uid);
		return;
	}

	target=getubyname(u,arg,temp);
	if (target) {
		if (time(0)-target->last_command<1200) {
			swrite(NULL,u,"You cannot smess someone who is less than 30 minutes idle.\n");
			FREE(uid);
			return;
		}
	}

	blah=mysql_real_escape_string(global.database,temp2,mes, strlen(mes));
	sprintf(temp,"INSERT INTO smess (ufrom,uto,message,recorded) VALUES(%ld, %ld, '%s',%ld)",u->object, *uid, temp2,(long)time(0));

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database insert Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		swrite(NULL,u,"[error] Database insert failed.\n");
		return;
	}
	pthread_mutex_unlock(&db_mutex);
	swrite(NULL,u,"[smess] Message sent.\n");
}

void show_smess(connection *u)
{
	int found=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	int ernum;
	char temp[TMP_SIZE];
	char *value=NULL;
	unsigned long lastsmess=0;

	value=get_attribute(u->object, "lastsmess");
	if (value) {
		lastsmess=atol(value);
		FREE(value);
	}	
	sprintf(temp,"SELECT id,message,ufrom,recorded FROM smess WHERE uto=%ld AND id>%ld ORDER BY recorded DESC",u->object,lastsmess);

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database insert Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		swrite(NULL,u,"[error] Database select failed.\n");
		return;
	}

	result = mysql_store_result(global.database);
	while((row = mysql_fetch_row(result))) {
		found++;
		if (found==1) {
			format_bar(u, "Your saved messages",temp);
			swrite(NULL,u,temp);
			set_attribute(u->object, "lastsmess", row[0]);
		}
		value=db_get_obj_name(atol(row[2]));
		sprintf(temp,"^G- ^N%s (%s^A#%s^N, sent %s)\n",row[1],value?value:"unknown",row[0],daysago(time(0)-atol(row[3])));
		swrite(NULL,u,temp);
		if (value) FREE(value);
	}
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);

	if (found) {
		format_bar(u, NULL,temp);
		swrite(NULL,u,temp);
	}	

}

void event(connection *u,short id, char *passed, char **ret)
{
	if (id==E_POSTCOMMAND||id==E_CONNECT) {
		show_smess(u);
	}
}

int init()
{
	add_command("smess",&smess,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("smess",NULL);
	return 1;
}

char *version()
{
	return "Messaging system (c) 2004 Karl Bastiman $Revision: 2.1 $";
}
