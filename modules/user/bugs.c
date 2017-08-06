/* $Id: bugs.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

mode bugmode;
extern pthread_mutex_t db_mutex;

void register_bug(connection *u, char class, char *data)
{
        int ernum;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
        char temp[TMP_SIZE];
	char *data2=NULL;
        int rowcount;
	unsigned long id;
	unsigned long blah=0;

	data2=MALLOC(strlen(data)*2+2,"register_bug:data2");
	data2[0]='\0';
	if (data[0]!='\0') {
		blah=mysql_real_escape_string(global.database,data2,data, strlen(data));
	}
 

	pthread_mutex_lock(&db_mutex);
        strcpy(temp,"SELECT ticket from bugs order by ticket desc");

        ernum = mysql_query(global.database,temp);
        if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
		FREE(data2);
                return;
        }
        result = mysql_store_result(global.database);
        rowcount = mysql_num_rows(result);
	if(rowcount==0)
	{
		id=1;
        } else {
		row = mysql_fetch_row(result);
		id=atol(row[0])+1;
	}
        MYSQL_FREE(result);
	sprintf(temp,"insert into bugs set class='%c',ticket='%lu',seq=0,user='%lu',posted=now(),comment='%s',status='N'",
					class,id,u->object,data2);
        ernum = mysql_query(global.database,temp);
        if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
		FREE(data2);
                return;
        }
	FREE(data2);
        pthread_mutex_unlock(&db_mutex);
        return;
}

void c_bug(connection *u, char *arg)
{
	if(!arg)
	{
		swrite(NULL,u,"Usage: bug <text>\n");
		return;
	}
	register_bug(u,'B',arg);
	swrite(NULL,u,"Bug registered, thanks.\n");
}

void c_idea(connection *u, char *arg)
{
	if(!arg)
	{
		swrite(NULL,u,"Usage: idea <text>\n");
		return;
	}
	register_bug(u,'I',arg);
	swrite(NULL,u,"Idea registered, thanks.\n");
}

void c_typo(connection *u, char *arg)
{
	if(!arg)
	{
		swrite(NULL,u,"Usage: typo <text>\n");
		return;
	}
	register_bug(u,'T',arg);
	swrite(NULL,u,"Typo registered, thanks.\n");
}

void b_list(connection *u, char *arg)
{
	int resolved=0,ernum;
	char type='X';
	char temp[TMP_SIZE];
	char *name=NULL;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;

	if(!arg)
	{
		swrite(NULL,u,"Usage: list [all] {bugs|ideas|typos}\n");
		return;
	}
	if(sscanf(arg,"all %s",temp)==1)
	{
		if(!strcasecmp(temp,"bugs")) { type='B'; }
		if(!strcasecmp(temp,"typos")) { type='T'; }
		if(!strcasecmp(temp,"ideas")) { type='I'; }
		resolved=1;
	} else {
		if(!strcasecmp(arg,"bugs")) { type='B'; }
		if(!strcasecmp(arg,"typos")) { type='T'; }
		if(!strcasecmp(arg,"ideas")) { type='I'; }
	}
	if(type=='X')
	{
		swrite(NULL,u,"Usage: list [all] {bugs|ideas|typos}\n");
		return;
	}
	if(resolved==1)
	{
		sprintf(temp,"SELECT user,status,date_format(posted,'%%d/%%m/%%Y'),ticket,comment FROM bugs where class='%c' and seq=0 order by posted desc;",type);
	} else {
		sprintf(temp,"SELECT user,status,date_format(posted,'%%d/%%m/%%Y'),ticket,comment FROM bugs where class='%c' and seq=0 and status='N' order by posted desc;",type);
	}

        pthread_mutex_lock(&db_mutex);

        ernum = mysql_query(global.database,temp);
        if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	swrite(NULL,u,"^YUsername   Ticket     Date  Status^N\n");
        result = mysql_store_result(global.database);
	while((row = mysql_fetch_row(result)))
	{
		name=db_get_obj_name(atoi(row[0]));
		sprintf(temp,"%-10s %-10s %-10s %s\n",name,row[3],row[2],row[1]);
		swrite(NULL,u,temp);
		sprintf(temp,"        %s\n",row[4]);
		swrite(NULL,u,temp);
		FREE(name);
	}
 	MYSQL_FREE(result);
	pthread_mutex_unlock(&db_mutex);
} 

void b_resolve(connection *u, char *arg)
{
	char temp[TMP_SIZE];
        int rowcount;
        MYSQL_RES *result=NULL;
	int ernum;

	if(!arg)
	{
		swrite(NULL,u,"Usage: resolve <ticket>\n");
		return;
	}
        pthread_mutex_lock(&db_mutex);
        sprintf(temp,"select seq from bugs where ticket='%s' and status='N'",arg);
                                                                                                                             
        ernum = mysql_query(global.database,temp);
        if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
        result = mysql_store_result(global.database);
        rowcount = mysql_num_rows(result);
        MYSQL_FREE(result);
        if(rowcount==0)
        {
		swrite(NULL,u,"No such ticket to resolve.\n");
		pthread_mutex_unlock(&db_mutex);
		return;
        }

	sprintf(temp,"UPDATE bugs SET status='R' where ticket='%s'",arg);
        ernum = mysql_query(global.database,temp);
        if(ernum!=0)
        {
                debug("Database Update Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	swrite(NULL,u,"Ticket resolved.\n");
	pthread_mutex_unlock(&db_mutex);
	
}

void b_deny(connection *u, char *arg)
{
	char temp[TMP_SIZE];
        int rowcount;
        MYSQL_RES *result=NULL;
	int ernum;

	if(!arg)
	{
		swrite(NULL,u,"Usage: deny <ticket>\n");
		return;
	}
        pthread_mutex_lock(&db_mutex);
        sprintf(temp,"select seq from bugs where ticket='%s' and status='N'",arg);
                                                                                                                             
        ernum = mysql_query(global.database,temp);
        if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
        result = mysql_store_result(global.database);
        rowcount = mysql_num_rows(result);
        MYSQL_FREE(result);
        if(rowcount==0)
        {
		swrite(NULL,u,"No such ticket to deny.\n");
		pthread_mutex_unlock(&db_mutex);
		return;
        }

	sprintf(temp,"UPDATE bugs SET status='D' where ticket='%s'",arg);
        ernum = mysql_query(global.database,temp);
        if(ernum!=0)
        {
                debug("Database Update Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	swrite(NULL,u,"Ticket denied.\n");
	pthread_mutex_unlock(&db_mutex);
	
}

void bugstart(connection *u, char *arg)
{
	swrite(NULL,u,"Entering BUG mode.\n");
	u->usermode=&bugmode;
}

void bugend(connection *u, char *arg)
{
	swrite(NULL,u,"Leaving BUG mode.\n");
	u->usermode=NULL;
}

int init()
{
	bugmode.prompt=MALLOC(128,"init:bugmode.prompt");
	strcpy(bugmode.prompt,"bugs>");
	add_command("editbug",&bugstart,NULL,NULL);
	add_command("commands",&listcommands,NULL,&bugmode);
	add_command("end",&bugend,NULL,&bugmode);
	add_command("bug",&c_bug,NULL,NULL);
	add_command("idea",&c_idea,NULL,NULL);
	add_command("typo",&c_typo,NULL,NULL);
	add_command("list",&b_list,NULL,&bugmode);
	add_command("resolve",&b_resolve,NULL,&bugmode);
	add_command("deny",&b_deny,NULL,&bugmode);
	return 1;
}

int uninit()
{
	connection *sweep;
	for(sweep=users; sweep; sweep=sweep->next)
	{
		if(sweep->usermode==&bugmode)
		{
			return 0;
		}
	}
	FREE(bugmode.prompt);
	remove_command("end",&bugmode);
	remove_command("editbug",NULL);
	remove_command("commands",&bugmode);
	remove_command("bug",NULL);
	remove_command("idea",NULL);
	remove_command("typo",NULL);
	remove_command("list",&bugmode);
	remove_command("resolve",&bugmode);
	remove_command("deny",&bugmode);
	return 1;
}

char *version()
{
	return "Bug code (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
