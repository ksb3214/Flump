/* $Id: news.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

extern pthread_mutex_t db_mutex;
int newscount=0;

void newsadd(connection *u, char *arg)
{
	int ernum;
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE*2];
	unsigned long blah=0;

	if (!arg) {
		swrite(NULL,u,"newsadd [message].\n");
		return;
	}	

	if (strlen(arg)>400) {
		swrite(NULL,u,"[news] Sorry but news postings are limited to 400 characters (normally 5 lines of text)\n");
		return;
	}

	blah=mysql_real_escape_string(global.database,temp2,arg, strlen(arg));
	sprintf(temp,"INSERT INTO news (message,recorded,who) VALUES('%s',%d,%ld)",temp2,time(0),u->object);

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
	swrite(NULL,u,"[news] New message added.\n");
}

void newsdel(connection *u, char *arg)
{
	unsigned long delid=0;
	int ernum;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"newsdel [id].\n");
		return;
	}	
	delid=atol(arg);
	sprintf(temp,"DELETE FROM news where id=%ld",delid);

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
	swrite(NULL,u,"[news] message deleted.\n");
}

void news(connection *u, char *arg)
{
	int found=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	int ernum;
	char temp[TMP_SIZE];
	char *value=NULL;

/*
	sprintf(temp,"SELECT id,message,recorded,who FROM news ORDER BY recorded ASC LIMIT -10");
*/
	sprintf(temp,"select * from (select id as sb1, message as sb2, recorded as sb3, who as sb4 from news order by recorded desc limit 0,10) as sb order by sb3 asc");

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
/*		if (time(0)-atol(row[2])>864000) continue;
*/
		found++;
		if (found==1) {
			format_bar(u, "News",temp);
			swrite(NULL,u,temp);
		}
		value=db_get_obj_name(atol(row[3]));
		sprintf(temp,"%s ^G- ^N%s (posted %s, by %s)\n",row[0],row[1],daysago(time(0)-atol(row[2])), value?value:"unknown");
/*
		sprintf(temp,"^G- ^N%s (%s^A#%s^N, posted %s)\n",row[1],value?value:"unknown",row[0],daysago(time(0)-atol(row[2])));
*/
		swrite(NULL,u,temp);
		strcpy(temp, row[2]);
		if (value) FREE(value);
	}
	set_attribute(u->object, "newsstamp", temp);
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);

	if (found) {
		format_bar(u, "Post using the 'newsadd' command",temp);
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
	char *muff;
	int m=0;

	newscount++;
	if (newscount<20) return;
	newscount=0;

	for (sweep=users;sweep;sweep=sweep->next)
	{
		if (sweep->quiet) continue;
		if (time(0)-sweep->last_command>600) continue;
		m=0;
		muff=get_attribute(sweep->object,"muffle_news");
		if(muff)
		{
			if(*muff=='Y')
			{
				m=1;
			}
			FREE(muff);
		}
		if (m) continue;

		sprintf(temp,"SELECT recorded FROM news ORDER BY recorded DESC LIMIT 1");

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
			value=get_attribute(sweep->object,"newsstamp");
			if (rec>atol(value?value:"0")) {
				swrite(NULL,sweep,"-> There are ^GNEW^N news entries. (type ^Pnews^N to read)\n");
				set_attribute(sweep->object,"newsstamp",row[0]);
			}
			if (value) FREE(value);
		}
		MYSQL_FREE(result);
        	pthread_mutex_unlock(&db_mutex);
	}
}

int init()
{
	add_command("news",&news,NULL,NULL);
	add_command("newsadd",&newsadd,NULL,NULL);
	add_command("newsdel",&newsdel,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("news",NULL);
	remove_command("newsadd",NULL);
	remove_command("newsdel",NULL);
	return 1;
}

char *version()
{
	return "News system (c) 2005 Karl Bastiman $Revision: 2.0 $";
}
