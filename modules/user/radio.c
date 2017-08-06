/* $Id: radio.c,v 2.2 2005/07/28 12:12:45 ksb Exp $ */
/* $Revision: 2.2 $ */

#include "flump.h"

#define ARTIST 1
#define TITLE 2
#define ALBUM 3
#define MP3DIR "/mnt/mp3/"

extern pthread_mutex_t db_mutex;
int ticker=0;
int ticker2=0;
time_t tickmtime=0;
char curtrack[TMP_SIZE];
time_t genretime=0;

/* does not obey ignore */
void sradio(connection *u, char *txt)
{
	connection *sweep=NULL;
	char *muff=NULL;
	time_t ix=0;


	debug("swall");
	for(sweep=users; sweep; sweep=sweep->next)
	{
		if (sweep==u) continue;
		if (sweep->quiet) continue;
		muff=get_attribute(sweep->object,"muffle_radio");
		if (muff) {
			if (muff[0]=='Y') {
				FREE(muff);
				continue;
			}
			FREE(muff);
		}	
		ix=(time(0)-sweep->last_command);
		if (ix>7200) continue;
		swrite(NULL,sweep,txt);
	}
	debug("swall--");
}

unsigned long numingenre(char *genre)
{
	char temp[TMP_SIZE];
	unsigned long count=0;
	int ernum=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;

	sprintf(temp,"select count(id) from mp3 where genre='%s'",genre);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
       	{
               debug("Database Select Failed:");
               debug((char *)mysql_error(global.database));
                pthread_mutex_unlock(&db_mutex);
                return 0;
        }
        pthread_mutex_unlock(&db_mutex);
	result = mysql_store_result(global.database);
	while((row = mysql_fetch_row(result))) {
		count+=atol(row[0]);
	}
	MYSQL_FREE(result);
	return count;
}

void radiogenre(connection *u, char *arg)
{
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE];
	char genre[TMP_SIZE];
	int ernum;
	int found=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	FILE *fp;

	if (!arg) {
		format_bar(u, "Available Genres",temp);
		swrite(NULL,u,temp);
		sprintf(temp,"^G%-20s^N   ^G%s^N\n","Genre","Tracks");
		swrite(NULL,u,temp);
		sprintf(temp,"select genre,count(id) as m from mp3 where genre !='' group by genre order by m desc");
		ernum = mysql_query(global.database,temp);
		if(ernum!=0)
       		{
	                debug("Database Select Failed:");
	                debug((char *)mysql_error(global.database));
	                pthread_mutex_unlock(&db_mutex);
	                return;
	        }
	        pthread_mutex_unlock(&db_mutex);
		result = mysql_store_result(global.database);
		while((row = mysql_fetch_row(result))) {
			sprintf(temp,"%-20s : %s\n",row[0],row[1]);
			swrite(NULL,u,temp);
		}
		MYSQL_FREE(result);

		if (!(fp=fopen("/tmp/genre","r"))) {
			strcpy(genre,"All");
		}
		else
		{
			fgets(genre,255,fp);
			fclose(fp);
		}
		while (strchr(genre,'\n')) {
			*strchr(genre,'\n')='\0';
		}	
		sprintf(temp2,"Current genre: %s",genre);
		format_bar(u, temp2,temp);
		swrite(NULL,u,temp);
	}
	else
	{
		if (genretime-time(0)>0) {
			sprintf(temp,"Genre is set for another %ld minutes.\n",(long)((genretime-time(0))/60)+1);
			swrite(NULL,u,temp);
			return;
		}

		if (strcasecmp(arg,"all")==0) {
			sprintf(temp,"[Radio] Genre is now ^GAll^N for random play (set by %s)\n",u->name);
			sradio(NULL,temp);
			unlink("/tmp/genre");
			genretime=time(0)+1800;
			return;
		}

		sprintf(temp,"select distinct genre from mp3 where genre='%s'",arg);
		ernum = mysql_query(global.database,temp);
		if(ernum!=0)
       		{
	                debug("Database Select Failed:");
	                debug((char *)mysql_error(global.database));
	                pthread_mutex_unlock(&db_mutex);
	                return;
	        }
	        pthread_mutex_unlock(&db_mutex);
		result = mysql_store_result(global.database);
		while((row = mysql_fetch_row(result))) {
			found=1;
		}
		MYSQL_FREE(result);

		if (!found) {
			swrite(NULL,u,"No such genre. (type radiogenre for a list)\n");
			return;
		}
		
		if (!(fp=fopen("/tmp/genre","w"))) {
			printf("File error /tmp/genre\n");
			return;
		}

		fprintf(fp,"%s\n",arg);
		fclose(fp);

		sprintf(temp,"[Radio] Genre set to ^G%s^N for random play. (by %s)\n",arg,u->name);
		sradio(NULL,temp);
		genretime=time(0)+1800;
	}	
}

/* artist <text>
** title <text>
*/
void radiosearch(connection *u, char *arg)
{
	char temp[TMP_SIZE];
	int searchtype=0;
	char *sp=NULL;
	char stype[TMP_SIZE];
	int ernum;
	int found=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	
	if (arg) {
		sp=strchr(arg,' ');
		if (sp) {
			*sp='\0';
			sp++;
			while (*sp==' ') sp++;
			if (strlen(sp)>2) {
				if (strcasecmp(arg,"artist")==0) searchtype=ARTIST;
				if (strcasecmp(arg,"title")==0) searchtype=TITLE;
				if (strcasecmp(arg,"album")==0) searchtype=ALBUM;
			}
		}
	}

	if (searchtype==0) {
		swrite(NULL,u,"Syntax: radiosearch {artist|title|album} search-text\n");
		swrite(NULL,u,"Search strings must be at least 3 chars.\n");
		return;
	}

	switch (searchtype) {
		case ARTIST : strcpy(stype,"artist"); break;
		case TITLE : strcpy(stype,"title"); break;
		case ALBUM : strcpy(stype,"album"); break;
		default : strcpy(stype,"title"); break;
	}

	format_bar(u, "Search results",temp);
	swrite(NULL,u,temp);

	sprintf(temp,"^G%6s %-30s %-40s^N\n","ID","Artist","Title");
	swrite(NULL,u,temp);
	pthread_mutex_lock(&db_mutex);
	sprintf(temp,"SELECT id,artist,title FROM mp3 WHERE %s like '%%%s%%'",stype,sp);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	result = mysql_store_result(global.database);
	while((row = mysql_fetch_row(result))) {
		found=1;
		sprintf(temp,"%6s %-30s %-40s\n",row[0],row[1],row[2]);
		swrite(NULL,u,temp);
	}
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);

	if (!found) {
		swrite(NULL,u,"No tracks found, sorry.\n");
	}

	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
}

/* <id>
*/
void radioadd(connection *u, char *arg)
{
	long narg=0;
	int num_fields=0;
	char temp[TMP_SIZE];
	int ernum=0;
	int min=0;
	int sec=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;

	if (!arg) {
		swrite(NULL,u,"Syntax: radioadd song-id\n");
		return;
	}

	narg=atol(arg);
	if (narg<1) {
		swrite(NULL,u,"Value must be a positive numeric.\n");
		return;
	}

	pthread_mutex_lock(&db_mutex);
	sprintf(temp,"SELECT id,artist,title,min,sec FROM mp3 WHERE id=%ld",narg);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                pthread_mutex_unlock(&db_mutex);
                return;
        }
        pthread_mutex_unlock(&db_mutex);
	result = mysql_store_result(global.database);
	num_fields = mysql_num_rows(result);
	if (num_fields<1) {
		swrite(NULL,u,"No such track.\n");
		return;
	}
	row = mysql_fetch_row(result);

	sprintf(temp,"insert into queue values(%ld,%ld,%ld,99)",narg,u->object,(long)time(0));
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	sprintf(temp,"^B%s - %s^N queued.\n",row[1],row[2]);
	swrite(NULL,u,temp);
	min=atoi(row[3]);
	sec=atoi(row[4]);
	sprintf(temp,"[Radio] ^B%s - %s ^b[%d:%02d]^N queued by %s.\n",row[1],row[2],min,sec,u->name);
	sradio(u,temp);
	MYSQL_FREE(result);
}

/* returns currently playing and the queue as it stands
*/
void radioinfo(connection *u, char *arg)
{
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE];
        MYSQL_RES *result2=NULL;
        MYSQL_ROW row2;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	char *uname=NULL;
	int ernum=0;
	int num_fields=0;
	char xartist[TMP_SIZE];
	char xtitle[TMP_SIZE];
	time_t rsecs=0;


	if (strlen(curtrack)>50) curtrack[50]='\0';
	sprintf(temp2,"Currently Playing - %s",curtrack);
	format_bar(u, temp2,temp);
	swrite(NULL,u,temp);

	sprintf(temp,"^G%6s %-20s %-30s %-20s^N\n","ID","Artist","Title","Queued by");
	swrite(NULL,u,temp);

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,"SELECT id,user FROM queue ORDER BY queued");
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	result = mysql_store_result(global.database);
	num_fields = mysql_num_fields(result);
	while ((row = mysql_fetch_row(result))&&(num_fields)) {
		sprintf(temp,"SELECT artist,title FROM mp3 WHERE id=%s",row[0]);
		ernum=mysql_query(global.database,temp);
		if(ernum!=0)
		{
               		 debug("Database Select Failed:");
		         debug((char *)mysql_error(global.database));
               		 pthread_mutex_unlock(&db_mutex);
		         return;
       		}
		result2=mysql_store_result(global.database);
		row2=mysql_fetch_row(result2);
		uname=db_get_obj_name(atol(row[1]));
		strcpy(xartist,row2[0]);
		strcpy(xtitle,row2[1]);
		xartist[20]='\0';
		xtitle[30]='\0';
		sprintf(temp,"%6s %-20s %-30s %-20s\n",row[0],xartist,xtitle,uname?uname:"Unknown");
		if (uname) FREE(uname);
		swrite(NULL,u,temp);
		MYSQL_FREE(result2);
	}	
	MYSQL_FREE(result);

	ernum = mysql_query(global.database,"SELECT count(id), sum(min), sum(sec) FROM mp3");
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	result = mysql_store_result(global.database);
	row = mysql_fetch_row(result);
	rsecs=(atol(row[1])*60)+atol(row[2]);
	sprintf(temp,"^GTracks currently stored^N : %s    ^GTotal playtime^N : %s\n",row[0],timeval(rsecs));
	swrite(NULL,u,temp);
	MYSQL_FREE(result);

	sprintf(temp2,"Listen in - http://%s:PASSWORD@radio.amber.org.uk:8000/flump",u->name);
	format_bar(u, temp2,temp);
	swrite(NULL,u,temp);
        pthread_mutex_unlock(&db_mutex);
}

void radiotop10(connection *u, char *arg)
{
	char temp[TMP_SIZE];
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	int ernum=0;
	char xartist[TMP_SIZE];
	char xtitle[TMP_SIZE];
	int pos=0;

	format_bar(u, "Flump top 10 requests",temp);
	swrite(NULL,u,temp);

	sprintf(temp,"^G%s %6s %-20s %-30s %s^N\n","Pos", "ID","Artist","Title","Requested");
	swrite(NULL,u,temp);

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,"SELECT id,artist,title,played FROM mp3 ORDER BY played DESC LIMIT 0,10");
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	result = mysql_store_result(global.database);
	while ((row = mysql_fetch_row(result))) {
		pos++;
		strcpy(xartist,row[1]);
		strcpy(xtitle,row[2]);
		xartist[20]='\0';
		xtitle[30]='\0';
		sprintf(temp,"%3d %6s %-20s %-30s %8s\n",pos,row[0],xartist,xtitle,row[3]);
		swrite(NULL,u,temp);
	}	
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);
	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
}

void tick()
{
	struct stat sb;
	FILE *fp=NULL;
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE];
	char *uname=NULL;
        MYSQL_RES *result2=NULL;
        MYSQL_ROW row2;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	int found=0;
	int ernum=0;
	int num_fields=0;
	int min=0;
	int sec=0;

	ticker2++;
	if (ticker2>2) {
		ticker2=0;
		pthread_mutex_lock(&db_mutex);
		ernum = mysql_query(global.database,"SELECT id,user FROM queue WHERE shown IS NULL ORDER BY queued");
		if(ernum!=0)
        	{
                	debug("Database Select Failed:");
                	debug((char *)mysql_error(global.database));
                	pthread_mutex_unlock(&db_mutex);
                	return;
        	}
		result = mysql_store_result(global.database);
		num_fields = mysql_num_fields(result);
		while ((row = mysql_fetch_row(result))&&(num_fields)) {
			sprintf(temp,"SELECT id,artist,title,min,sec FROM mp3 WHERE id=%s",row[0]);
			ernum = mysql_query(global.database,temp);
			if(ernum!=0)
       			{
       			        debug("Database Select Failed:");
       			        debug((char *)mysql_error(global.database));
       			        pthread_mutex_unlock(&db_mutex);
       			        return;
       			}
			result2 = mysql_store_result(global.database);
			row2 = mysql_fetch_row(result2);
			uname=db_get_obj_name(atol(row[1]));
			min=atoi(row2[3]);
			sec=atoi(row2[4]);
			sprintf(temp,"[Radio] ^B%s - %s ^b[%d:%02d]^N queued by %s. (^gvia website^N)\n",row2[1],row2[2],min,sec,uname?uname:"Unknown");
			sradio(NULL,temp);
			FREE(uname);
			found=1;
			MYSQL_FREE(result2);
		}	
		MYSQL_FREE(result);
		if (found) {
			ernum = mysql_query(global.database,"UPDATE queue SET shown=99 WHERE shown IS NULL ");
			if(ernum!=0)
       	 		{
                		debug("Database Select Failed:");
                		debug((char *)mysql_error(global.database));
                		pthread_mutex_unlock(&db_mutex);
                		return;
        		}
		}	
       		pthread_mutex_unlock(&db_mutex);
	}	

	ticker++;
	if (ticker<10) return;
	ticker=0;
	
	if (stat("/tmp/playing",&sb)==0) {
		if (sb.st_mtime==tickmtime) return;
		tickmtime=sb.st_mtime;
		if (!(fp=fopen("/tmp/playing","r"))) {
			return;
		}
		fgets(temp,255,fp);
		while (strchr(temp,'\n')) {
			*strchr(temp,'\n')='\0';
		}	
		strcpy(curtrack,temp);
		sprintf(temp2,"[Radio] Playing ... ^C%s^N\n",temp);
		sradio(NULL,temp2);
		fclose(fp);
	}	
}

int init()
{
	add_command("radiosearch",&radiosearch,NULL,NULL);
	add_command("radioadd",&radioadd,NULL,NULL);
	add_command("radioinfo",&radioinfo,NULL,NULL);
	add_command("radiogenre",&radiogenre,NULL,NULL);
	add_command("radiotop10",&radiotop10,NULL,NULL);
	curtrack[0]='\0';
	return 1;
}

int uninit()
{
	remove_command("radiosearch",NULL);
	remove_command("radioadd",NULL);
	remove_command("radioinfo",NULL);
	remove_command("radiogenre",NULL);
	remove_command("radiotop10",NULL);
	return 1;
}

char *version()
{
	return "Radio system (c) 2004 Karl Bastiman $Revision: 2.2 $";
}
