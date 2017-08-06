/* $Id: info.c,v 2.5 2005/07/28 10:46:37 ksb Exp $ */
/* $Revision: 2.5 $ */

#include "flump.h"

extern pthread_mutex_t db_mutex;

void iwho(connection *u, char *arg)
{
	MYSQL_RES *result=NULL;
	MYSQL_ROW row;
	int ernum;
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE];
	char *ret=NULL;
	unsigned long hasi=0;
	connection *target=NULL;

	if (!arg) {
		target=u;
	}
	else
	{
		target=getubyname(u,arg,temp);
		if (!target) {
			swrite(NULL,u,temp);
			return;
		}
	}

	if (u==target) {
		format_bar(u,"Users you have invited",temp);
	}
	else
	{
		sprintf(temp2, "Users %s has invited", target->name);
		format_bar(u,temp2,temp);
	}
	swrite(NULL,u,temp);

	sprintf(temp,"select object from attributes where name='invitee' and value='%ld'",target->object);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database insert Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		swrite(NULL,u,"[error] Database select failed. (inform an admin)\n");
		return;
	}
	temp[0]='\0';
	result = mysql_store_result(global.database);
	while((row = mysql_fetch_row(result))) {
		hasi=atol(row[0]);
		ret=db_get_obj_name(hasi);
		if (!ret) continue;
		if (strlen(temp)+strlen(ret)+5>u->twidth) {
			strcat(temp,"\n");
			swrite(NULL,u,temp);
			temp[0]='\0';
		}
		if (temp[0]!='\0') strcat(temp, ", ");
		strcat(temp,ret);
		FREE(ret);
	}
	if (temp[0]!='\0') {
		strcat(temp,"\n");
		swrite(NULL,u,temp);
		temp[0]='\0';
	}
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);

	format_bar(u,NULL,temp);
	swrite(NULL,u,temp);
	
}

void help(connection *u, char *arg) 
{
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	int ernum;
	char temp[TMP_SIZE];
	char *value=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a topic, help contents will give an overview.\n");
		return;
	}

	for (value=arg;*value!='\0';value++) {
		if (!isalpha(*value)) {
			swrite(NULL,u,"That's just silly.\n");
			return;
		}
	}

	sprintf(temp,"SELECT entry FROM help WHERE title='%s'",arg);
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
	row = mysql_fetch_row(result);
	if (row) {
		format_bar(u,"Help",temp);
		swrite(NULL,u,temp);
		sprintf(temp,"%s\n",row[0]);
		swrite(NULL,u,temp);
		format_bar(u, "http://flump.amber.org.uk",temp);
		swrite(NULL,u,temp);
	}
	else
	{
		swrite(NULL,u,"No help on that topic, sorry.\n");
	}	
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);

}

void main_version(connection *u, char *arg)
{
	lib *sweep=NULL;
	char temp[TMP_SIZE];
	sprintf(temp,"^BFlump^N core version ^*2.0^N.\n");
	swrite(NULL,u,temp);
        for(sweep=libs; sweep; sweep=sweep->next)
        {
                if(sweep->version)
                {
                        sprintf(temp,"  %s\n",sweep->version());
                }
                swrite(NULL,u,temp);
        }

}

void credits(connection *u, char *arg)
{
	swrite(NULL,u,"Flump is developed by...\n");
	swrite(NULL,u,"  Karl Bastiman (kenny).\n");
	swrite(NULL,u,"Website by...\n");
	swrite(NULL,u,"  peter.\n");
	swrite(NULL,u,"Initial testing help from...\n");
	swrite(NULL,u,"  nutnut, dodger, sickle, goolies, david and peter.\n");
}

void xxx(connection *u, char *arg)
{
	connection *target=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;
	char *dat2=NULL;
	char *dat3=NULL;
	char *dat4=NULL;
	char *fs=NULL;
	long oid=0;
	time_t tval=0;
	int last=0;
	int invites=0;
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	int ernum;
	int hasi=0;

	if (!arg) {
		target=u;
	}
	else
	{
		target=getubyname(u,arg,temp);
		if (!target) {
			swrite(NULL,u,temp);
			return;
		}
	}

	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
	dat=get_attribute(target->object,"title");
	sprintf(temp,"%s%s%s\n",target->name,dat?(*dat==','||*dat=='\''?"":" "):" ",dat?dat:"");
	if (dat) FREE(dat);
	swrite(NULL,u,temp);
	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);

	/* stuff */

	sprintf(temp,"^GGender^N: %s",getgen(target,"Male","Female","Neuter"));
	dat=get_attribute(target->object,"age");
	if (dat) {
		strcat(temp,", ^GAge^N: ");
		strcat(temp,dat);
		FREE(dat);
	}	
	strcat(temp,"\n");
	swrite(NULL,u,temp);
	
	sprintf(temp,"^GLogged in for^N: %s\n",timeval(time(0)-target->logged_in));
	swrite(NULL,u,temp);

	tval=0;
	dat=get_attribute(target->object,"truespod");
	if (dat) {
		tval=atol(dat);
		FREE(dat);
	}	
	sprintf(temp,"^GTrue spodtime^N: %s\n",timeval(tval+target->ctruespod));
	swrite(NULL,u,temp);

	tval=0;
	dat=get_attribute(target->object,"totalspod");
	if (dat) {
		tval=atol(dat);
		FREE(dat);
	}	
	sprintf(temp,"^GTotal spodtime^N: %s\n",timeval(tval+target->cspod));
	swrite(NULL,u,temp);

	dat=get_attribute(target->object,"email");
	if (dat) {
		dat2=get_attribute(target->object,"emailsee");
		if (dat2) {
			stolower(dat2);
			if (dat2[0]=='a') {
				sprintf(temp,"^GEmail^N: %s (public)\n",dat);
				swrite(NULL,u,temp);
			}
			if (dat2[0]=='f') {
				fs=get_attribute(target->object,"friends");
				if (fs) {
					if (in_delim(fs,u->name)) {
						sprintf(temp,"^GEmail^N: %s (friends)\n",dat);
						swrite(NULL,u,temp);
					}
					FREE(fs);
				}	
				if (target==u) {
					sprintf(temp,"^GEmail^N: %s (friends)\n",dat);
					swrite(NULL,u,temp);
				}	
			}
			if (dat2[0]=='p') {
				if (target==u) {
					sprintf(temp,"^GEmail^N: %s (private)\n",dat);
					swrite(NULL,u,temp);
				}	
			}
			FREE(dat2);
		}	
		else
		{
			if (target==u) {
				sprintf(temp,"^GEmail^N: %s (private)\n",dat);
				swrite(NULL,u,temp);
			}	
		}	
		FREE(dat);
	}	

	dat=get_attribute(target->object,"jabber");
	if (dat) {
		sprintf(temp,"^GJabber ID^N: %s\n",dat);
		swrite(NULL,u,temp);
		FREE(dat);
	}	

	dat=get_attribute(target->object,"invites");
	if (dat) {
		invites=atol(dat);
		FREE(dat);
	}	

	if (u==target) {
		sprintf(temp,"^GInvites left^N: %d (10 maximum)\n",invites);
		swrite(NULL,u,temp);
	}	

	dat=get_attribute(target->object,"itime");
	if (dat) {
		tval=atol(dat);
		sprintf(temp,"^GInvited since^N: %s",ctime(&tval));
		FREE(dat);
	}	
	else
	{
		sprintf(temp,"^GInvited since^N: [the beginning]\n");
	}	
	swrite(NULL,u,temp);

	dat=get_attribute(target->object,"invitee");
	dat2=NULL;
	if (dat) {
		oid=atol(dat);
		dat2=db_get_obj_name(oid);
	}	
	sprintf(temp,"^GInvited by^N: %s\n",dat2?dat2:dat?"[No longer a member]":"[is a founder member]");
	if (dat) FREE(dat);
	if (dat2) FREE(dat2);
	swrite(NULL,u,temp);

/* Some dodgy non cahed sql */

	sprintf(temp,"select count(*) from attributes where name='invitee' and value='%ld'",target->object);
	ernum = mysql_query(global.database,temp);
	if(ernum!=0)
	{
		debug("Database insert Failed:");
		debug((char *)mysql_error(global.database));
		debug(temp);
		pthread_mutex_unlock(&db_mutex);
		swrite(NULL,u,"[error] Database select failed. (inform an admin)\n");
		return;
	}

	result = mysql_store_result(global.database);
	row = mysql_fetch_row(result);
	if (row) {
		hasi=atoi(row[0]);
	}
	MYSQL_FREE(result);
        pthread_mutex_unlock(&db_mutex);
	
	sprintf(temp, "^GHas invited^N: %d other user%s %s\n", hasi, hasi==1?"":"s",hasi>0?"(type ^Yiwho^N to see who)":"");
	swrite(NULL,u,temp);

/* ----------------------------------------------------------- */



	dat=get_attribute(target->object,"c4win");
	dat2=get_attribute(target->object,"c4winrow");
	dat3=get_attribute(target->object,"c4lose");
	dat4=get_attribute(target->object,"c4draw");
	sprintf(temp,"^GConnect 4^N: wins %s (%s in a row), losses %s, draw %s\n",dat?dat:"0",dat2?dat2:"0",dat3?dat3:"0",dat4?dat4:"0");
	swrite(NULL,u,temp);

	if (dat) FREE(dat);
	if (dat2) FREE(dat2);
	if (dat3) FREE(dat3);
	if (dat4) FREE(dat4);

	sprintf(temp,"^GTerminal type^N: %s,  ^GWidth^N: %d,  ^Gheight^N: %d\n",target->ttype, target->twidth, target->theight);
	swrite(NULL,u,temp);
	format_bar(u, NULL, temp);
	swrite(NULL,u,temp);

/*
	if(target==u) {
		execute_data(u,target->object,"examine",0);
	} else {
		execute_data(u,target->object,"examine",1);
	}
*/	
	if (target==u) {

		dat=get_attribute(target->object,"money");
		sprintf(temp,"^GMoney^N: %s ^Gflimps^N\n",dat?dat:"0");
		swrite(NULL,u,temp);
		if (dat) FREE(dat);

		dat=get_attribute(target->object,"bankrupt");
		sprintf(temp,"^GBankrupted^N: %s ^Gtimes^N\n",dat?dat:"0");
		swrite(NULL,u,temp);
		if (dat) FREE(dat);

		format_bar(u, NULL,temp);
		swrite(NULL,u,temp);

		dat=get_attribute(target->object,"logonmsg");
		if (dat) {
			sprintf(temp,"^GLogonmsg^N: %s %s\n",target->name,dat);
			swrite(NULL,u,temp);
			FREE(dat);
			last=1;
		}	

		dat=get_attribute(target->object,"logoffmsg");
		if (dat) {
			sprintf(temp,"^GLogoffmsg^N: %s %s\n",target->name,dat);
			swrite(NULL,u,temp);
			FREE(dat);
			last=1;
		}	

		if (last) {
			format_bar(u, NULL,temp);
			swrite(NULL,u,temp);
		}	
	}	
}

void finger(connection *u, char *arg)
{
	connection *target=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;
	char *dat2=NULL;
	char *fs=NULL;
	long oid=0;
	time_t tval=0;
	unsigned long *objno;

	if (!arg) {
		xxx(u,arg);
		return;
	}
	else
	{
		target=getubyname(u,arg,temp);
		if (target) {
			xxx(u,arg);
			return;
		}
	}

	/* okay if we are here the user wanted is offline */

	objno=db_get_objid(arg,OB_USER);

	if (!objno) {
		swrite(NULL,u,"That user doesn't exist.\n");
		return;
	}

	dat=get_attribute(*objno,"passwd");
	if (!dat) {
		swrite(NULL,u,"That user doesn't exist.\n");
		return;
	}
	FREE(dat);
	/* we have a valid object of an offline user */

	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
	dat=db_get_obj_name(*objno);
	sprintf(temp,"%s",dat?dat:"");
	if (dat) FREE(dat);
	dat=get_attribute(*objno,"title");
	strcat(temp,dat?(*dat==','||*dat=='\''?"":" "):" ");
	strcat(temp,dat?dat:"");
	strcat(temp,"\n");
	if (dat) FREE(dat);
	swrite(NULL,u,temp);
	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);

	/* stuff */

/*
	sprintf(temp,"^GGender^N: %s",getgen(target,"Male","Female","Neuter"));
	dat=get_attribute(target->object,"age");
	if (dat) {
		strcat(temp,", ^GAge^N: ");
		strcat(temp,dat);
		FREE(dat);
	}	
	strcat(temp,"\n");
	swrite(NULL,u,temp);
*/	
	dat=get_attribute(*objno,"laston");
	sprintf(temp,"^GLast logged in^N: %s %s\n",dat?timeval(time(0)-atol(dat)):"Never.",dat?"ago.":"");
	if (dat) FREE(dat);
	swrite(NULL,u,temp);

	tval=0;
	dat=get_attribute(*objno,"truespod");
	if (dat) {
		tval=atol(dat);
		FREE(dat);
	}	
	sprintf(temp,"^GTrue spodtime^N: %s\n",timeval(tval));
	swrite(NULL,u,temp);

	tval=0;
	dat=get_attribute(*objno,"totalspod");
	if (dat) {
		tval=atol(dat);
		FREE(dat);
	}	
	sprintf(temp,"^GTotal spodtime^N: %s\n",timeval(tval));
	swrite(NULL,u,temp);

	dat=get_attribute(*objno,"email");
	if (dat) {
		dat2=get_attribute(*objno,"emailsee");
		if (dat2) {
			stolower(dat2);
			if (dat2[0]=='a') {
				sprintf(temp,"^GEmail^N: %s (public)\n",dat);
				swrite(NULL,u,temp);
			}
			if (dat2[0]=='f') {
				fs=get_attribute(*objno,"friends");
				if (fs) {
					if (in_delim(fs,u->name)) {
						sprintf(temp,"^GEmail^N: %s (friends)\n",dat);
						swrite(NULL,u,temp);
					}
					FREE(fs);
				}	
			}
			FREE(dat2);
		}	
		FREE(dat);
	}	

	dat=get_attribute(*objno,"jabber");
	if (dat) {
		sprintf(temp,"^GJabber ID^N: %s\n",dat);
		swrite(NULL,u,temp);
		FREE(dat);
	}	

	dat=get_attribute(*objno,"itime");
	if (dat) {
		tval=atol(dat);
		sprintf(temp,"^GInvited since^N: %s",ctime(&tval));
		FREE(dat);
	}	
	else
	{
		sprintf(temp,"^GInvited since^N: [the beginning]\n");
	}	
	swrite(NULL,u,temp);

	dat=get_attribute(*objno,"invitee");
	dat2=NULL;
	if (dat) {
		oid=atol(dat);
		dat2=db_get_obj_name(oid);
	}	
	sprintf(temp,"^GInvited by^N: %s\n",dat2?dat2:dat?"[No longer a member]":"[is a founder member]");
	if (dat) FREE(dat);
	if (dat2) FREE(dat2);
	swrite(NULL,u,temp);


	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
	
	FREE(objno);
}

void timex(connection *u, char *arg)
{
	struct tm *ttm;
	time_t timenow;
	time_t diff;
	char temp[TMP_SIZE];
	char t2[TMP_SIZE];

	timenow=time(0);

	ttm=(struct tm *)localtime((time_t *)&timenow);

	sprintf(temp,"Flump local time is %02d:%02d on %02d/%02d/%02d.\n",ttm->tm_hour,ttm->tm_min,ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900);
	swrite(NULL,u,temp);

	diff = timenow - global.boot_time;

	sprintf(t2,"Flump has been up for %s\n",timeval(diff));
	swrite(NULL,u,t2);
	sprintf(t2,"There have been %ld connection%s since flump booted.\n",global.logins_since,global.logins_since==1?"":"s");
	swrite(NULL,u,t2);
	sprintf(t2,"In that time flump has seen a maximum of %d user%s at any one time.\n",global.max_on, global.max_on==1?"":"s");
	swrite(NULL,u,t2);
	
}

void idle(connection *u, char *arg)
{
	connection *sweep=NULL;
	connection *t=NULL;
	char f[1024];
	int i;
	char temp[TMP_SIZE];
	char *title=NULL;
	unsigned long *objid=NULL;
	int ihour=0,imin=0i,iminx=0,isec=0;
	time_t ix=0;
	char *im=NULL;
	char gdot[11];
	int count=0;

	if (arg) {
		t=getubyname(u,arg,temp);
		if (t) {
			if (t==u) {
				swrite(NULL,u,"You are not idle, possibly a little mad though.\n");
				return;
			}
			ix=(time(0)-t->last_command);
		        imin=(int)(ix/60);
			isec=(int)(ix%60);
			sprintf(temp,"%s has been idle for %d minutes and %d seconds.\n",t->name,imin,isec);
			swrite(NULL,u,temp);
		}
		else
		{
			swrite(NULL,u,temp);
		}
		return;
	}	
			
	i=0;
	for(sweep=users; sweep; sweep=sweep->next) {
		if (sweep->quiet) continue;
		i++;
	}	

	sprintf(f,"There %s %d user%s connected at the moment",i==1?"is":"are",i,i==1?"":"s");
	format_bar(u, f,temp);
	swrite(NULL,u,temp);
	for(sweep=users; sweep; sweep=sweep->next)
	{
		if (sweep->quiet) continue;
		ix=(time(0)-sweep->last_command);
		ihour=(int)(ix/(60*60));
	        iminx=(int)((ix%(60*60))/60);
	        imin=(int)(ix/60);
		isec=(int)(ix%60);

		objid=db_get_objid(sweep->name,OB_USER);
		if (objid) {
			/* if we dont get here it's fucked anyway */
			title=get_attribute(*objid,"idlemsg");
			if (!title) {
				title=MALLOC(2*sizeof(char),"idle:title");
				title[0]='\0';
			}	
			FREE(objid);
		}	
		im=get_attribute(sweep->object,"idlemsg");
		if (im) {
			if (im[0]=='\0') {
				FREE(im);
				im=NULL;
			}
		}	
		strcpy(gdot,"         ");
		count=1; /* no # for under a minute */
		while (count<10) {
			if (count<=(int)(imin/2)) {
				gdot[count-1]='#';
			}
			else
			{
				gdot[count-1]=' ';
			}
			count++;
		}	
		
		sprintf(temp,"%4d:%02d:%02d [^G%s^N^R%s^N] %s%s%s\n",ihour,iminx,isec,gdot,(int)(imin/2)>9?"^R#^N":"^R ^N",sweep->name,*title==','||*title=='\''?"":" ",title);
		swrite(NULL,u,temp);
		if (im) FREE(im);
		FREE(title);
	}
	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
}

int init()
{
	add_command("version",&main_version,NULL,NULL);
	add_command("credits",&credits,NULL,NULL);
	add_command("time",&timex,NULL,NULL);
	add_command("idle",&idle,NULL,NULL);
	add_command("x",&xxx,NULL,NULL);
	add_command("finger",&finger,NULL,NULL);
	add_command("help",&help,NULL,NULL);
	add_command("iwho",&iwho,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("version",NULL);
	remove_command("credits",NULL);
	remove_command("time",NULL);
	remove_command("idle",NULL);
	remove_command("x",NULL);
	remove_command("finger",NULL);
	remove_command("help",NULL);
	remove_command("iwho",NULL);
	return 1;
}

char *version()
{
	return "Info system (c) 2004 Karl Bastiman $Revision: 2.5 $";
}
