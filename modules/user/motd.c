/* $Id: motd.c,v 2.1 2005/07/07 16:27:09 ksb  Exp $ */
/* $Revision: 2.1 $ */

#include "flump.h"

extern pthread_mutex_t db_mutex;
int motdcount=0;

void motdadd(connection *u, char *arg)
{
	int ernum;
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE*2];
	unsigned long blah=0;

	if (!arg) {
		swrite(NULL,u,"motdadd [message].\n");
		return;
	}	

	blah=mysql_real_escape_string(global.database,temp2,arg, strlen(arg));
	sprintf(temp,"INSERT INTO motd (message,recorded,who) VALUES('%s',%ld,%ld)",temp2,(long)time(0),u->object);

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
	swrite(NULL,u,"[motd] New message added.\n");
}

void motddel(connection *u, char *arg)
{
	unsigned long delid=0;
	int ernum;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"motddel [id].\n");
		return;
	}	
	delid=atol(arg);
	sprintf(temp,"DELETE FROM motd where id=%ld",delid);

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database insert Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		swrite(NULL,u,"[error] Database delete failed.\n");
		return;
	}
	pthread_mutex_unlock(&db_mutex);
	swrite(NULL,u,"[motd] message deleted.\n");
}

void motd(connection *u, char *arg)
{
	int found=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	int ernum;
	char temp[TMP_SIZE];
	char *value=NULL;

	sprintf(temp,"SELECT id,message,recorded,who FROM motd ORDER BY recorded DESC LIMIT 0,5");

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
		if (time(0)-atol(row[2])>864000) continue;
		found++;
		if (found==1) {
			format_bar(u, "Motd",temp);
			swrite(NULL,u,temp);
			set_attribute(u->object, "motdstamp", row[2]);
		}
		value=db_get_obj_name(atol(row[3]));
		sprintf(temp,"^G- ^N%s (%s^A#%s^N, posted %s)\n",row[1],value?value:"unknown",row[0],daysago(time(0)-atol(row[2])));
		swrite(NULL,u,temp);
		if (value) FREE(value);
	}
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);

	if (found) {
		format_bar(u, "http://flump.amber.org.uk",temp);
		swrite(NULL,u,temp);
	}	
	else
	{
		swrite(NULL,u,"No new messages at the moment.\n");
	}

}

void tick()
{
	int ernum;
	char temp[TMP_SIZE];
	char *value=NULL;
	connection *sweep;
	time_t rec=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;

	motdcount++;
	if (motdcount<20) return;
	motdcount=0;

	for (sweep=users;sweep;sweep=sweep->next)
	{
		if (sweep->quiet) continue;
		if (time(0)-sweep->last_command>600) continue;

		sprintf(temp,"SELECT recorded FROM motd ORDER BY recorded DESC LIMIT 0,1");

		pthread_mutex_lock(&db_mutex);
		ernum = mysql_query(global.database,temp);
		if(ernum!=0)
		{
			debug("Database insert Failed:");
			debug((char *)mysql_error(global.database));
			debug(temp);
			pthread_mutex_unlock(&db_mutex);
			swrite(NULL,sweep,"[error] Database tick select failed.\n");
			return;
		}

		result = mysql_store_result(global.database);
		row = mysql_fetch_row(result);
		if (row) {
			rec=atol(row[0]);
			value=get_attribute(sweep->object,"motdstamp");
			if (rec>atol(value?value:"0")) {
				swrite(NULL,sweep,"-> There are ^YNEW^N motd entries. (type ^Bmotd^N to read)\n");
				set_attribute(sweep->object,"motdstamp",row[0]);
			}
			if (value) FREE(value);
		}
		MYSQL_FREE(result);
        	pthread_mutex_unlock(&db_mutex);
	}
}

int init()
{
	add_command("motd",&motd,NULL,NULL);
	add_command("motdadd",&motdadd,NULL,NULL);
	add_command("motddel",&motddel,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("motd",NULL);
	remove_command("motdadd",NULL);
	remove_command("motddel",NULL);
	return 1;
}

char *version()
{
	return "Motd system (c) 2004 Karl Bastiman $Revision: 2.1 $";
}
