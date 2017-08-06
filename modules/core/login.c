/* $Id: login.c,v 2.1 2005/07/07 16:27:09 ksb Exp $ */
/* $Revision: 2.1 $ */

#include "flump.h"

void new_passwd(connection *u, char *arg);
void look(connection *u, char *);

void enter_talker(connection *u)
{
	int curon=0;
	connection *sweep=NULL;

	u->function=&run_command;
	u->quiet=0;
	u->logged_in=time(0);
	u->last_command=time(0);
	u->usermode=NULL;
	u->lastarg=NULL;
	u->prev_command=NULL;
	u->quickdis=0;
	global.logins_since++;
	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		curon++;
	}
	if (curon>global.max_on) global.max_on=curon;

	echo_on(u);
	swrite(NULL,u,"\n\nWelcome to Flump.\n\nMake sure you read ^Yhelp rules^N\n\n");
	look(u, NULL);
	swrite(NULL,u,"\n");
	raise_event(u,E_CONNECT,NULL,NULL);
	showprompt(u);
}

int check_name(char *name)
{
	char *l=NULL;

	if (!name) return 0;

	if (strlen(name)>MAX_NAME-1) return 0;

	for (l=name;*l!='\0';l++) {
		if (!isalpha(*l)) return 0;
	}	

	if (is_command(name)) return 0;

	return 1;
}

void get_passwd(connection *u, char *arg)
{
	char *first=NULL;
	connection *sweep;
	int oldfd;
	
	if (strlen(arg)<1) return;

	first=(char *)get_attribute(u->object,"passwd");
	if (!first) {
		swrite(NULL,u,"\nPassword problem, sorry!\n");
		disconnect(u);
		return;
	}

	if (strcmp(arg,first)!=0) {
		swrite(NULL,u,"\nPassword did not match, try again!\n");
		swrite(NULL,u,(char *)config_get("prompt_pass"));
		echo_off(u);
		u->function=&get_passwd;
	}	
	else
	{
		swrite(NULL,u,"\nPassword match!\n");
		for(sweep=users; sweep; sweep=sweep->next)
		{
			if((!strcasecmp(u->name,sweep->name)) && sweep!=u)
			{
				swrite(NULL,u,"\nReconnecting...\n\n");
				swrite(NULL,sweep,"\n\n ---- You have reconnected in another session ----\n\n");
				close(sweep->fd);
				oldfd=sweep->fd;
				sweep->fd = u->fd;
				u->fd=oldfd;;
				disconnect(u);
				raise_event(u,E_RECONNECT,NULL,NULL);
				echo_on(sweep);
				return;
			}
		}
		
		enter_talker(u);
	}
	FREE(first);
}

void check_newpasswd(connection *u, char *arg)
{
	char *first=NULL;

	if (strlen(arg)<1) return;

	first=(char *)get_attribute(u->object,"passwd");
	if (!first) {
		swrite(NULL,u,"\nPassword problem, sorry!\n");
		disconnect(u);
		return;
	}

	if (strcmp(arg,first)!=0) {
		swrite(NULL,u,"\nPasswords do not match, try again!\n");
		swrite(NULL,u,(char *)config_get("prompt_newpass1"));
		echo_off(u);
		u->function=&new_passwd;
	}	
	else
	{
		swrite(NULL,u,"\nPasswords match!\n");
		enter_talker(u);
	}
	FREE(first);
}

void new_passwd(connection *u, char *arg)
{
	if (strlen(arg)<1) return;

	set_attribute(u->object,"passwd",arg);
	set_attribute(u->object,"prompt",config_get("prompt_default"));
	swrite(NULL,u,"\n");
	swrite(NULL,u, (char *)config_get("prompt_newpass2"));
	echo_off(u);
	u->function=&check_newpasswd;
}

void invite(connection *u, char *arg)
{
	unsigned long *value=NULL;
	char temp[TMP_SIZE];
	char *pwd=NULL;
	char *dat=NULL;
	unsigned long invites=0;

	dat=get_attribute(u->object,"invites");
	if (dat) {
		invites=atol(dat);
		FREE(dat);
	}	

	if (!arg) {
		sprintf(temp,"You have %ld invite%s available.\n",invites,invites==1?"":"s");
		swrite(NULL,u,temp);
		return;
	}	

	if (invites<1) {
		sprintf(temp,"You do not have any invites left, sorry. (you get 1 invite per spodday)\n");
		swrite(NULL,u,temp);
		return;
	}

	stolower(arg);

	if (!check_name(arg)) {
		swrite(NULL,u,"Invalid name, try again.\n");
		return;
	}

	value=db_get_objid(arg,OB_USER);
	if (!value) {
		value=(unsigned long *)db_set_newobj(arg,OB_USER);
		if (!value) {
			swrite(NULL,u,"Sorry internal error, try again later!\n");
			return;
		}	
	}
	pwd=get_attribute(*value,"passwd");
	if (pwd) {
		FREE(pwd);
		FREE(value);
		swrite(NULL,u,"Already a resident.\n");
		return;
	}	
	pwd=get_attribute(*value,"itime");
	if (pwd) {
		if (time(0)-atol(pwd)<=7200) {
			swrite(NULL,u,"That user already has a valid invite.\n");
			FREE(pwd);
			FREE(value);
			return;
		}
		FREE(pwd);
	}	
	sprintf(temp,"%ld",(long)time(0));
	set_attribute(*value,"itime",temp);
	sprintf(temp,"%ld",u->object);
	set_attribute(*value,"invitee",temp);
	sprintf(temp,"An invite for %s is valid for 2 hours.\n",arg);
	swrite(NULL,u,temp);
	FREE(value);
	invites--;
	sprintf(temp,"%ld",invites);
	set_attribute(u->object,"invites",temp);
}

void login(connection *u, char *arg)
{
	unsigned long *value=NULL;
	char *pwd=NULL;
	char temp[TMP_SIZE];
	connection *sweep=NULL;
	int userson=0;
	struct tm *ttm;

	for(sweep=users; sweep; sweep=sweep->next) {
		if (sweep->quiet) continue;
		userson++;
	}	

	if (!arg) return;
	if (strlen(arg)<1) return;
	if (strlen(arg)>18) {
		swrite(NULL,u,"Name too long.\nTry again.\n");
		return;
	}

	strcpy(u->name,arg);
	stolower(u->name);
	/* check default stuff */
	if (strcmp(arg,"quit")==0) {
		disconnect(u);
		return;
	}

	if (strcmp(arg,"who")==0) {
		sprintf(temp,"\n\nThere are %d user%s on.\n",userson,userson==1?"":"s");
		swrite(NULL,u,temp);
		disconnect(u);
		return;
	}
	
	if (strcmp(arg,"stats_info")==0) {
		swrite(NULL,u,"\nurl: http://flump.amber.org.uk\n");
		swrite(NULL,u,"contact: flump@amber.org.uk\n");
		swrite(NULL,u,"connect: telnet://amber.org.uk:5000\n");
		swrite(NULL,u,"newbies: yes\n");
		swrite(NULL,u,"residents: yes\n");
		swrite(NULL,u,"description: Flump is invite only.\n");
		swrite(NULL,u,"maxusers: 200\n");
		sprintf(temp,"users: %d\n",userson);
		swrite(NULL,u,temp);
		ttm=(struct tm *)gmtime((time_t *)&global.boot_time);
		sprintf(temp,"bootdate: %04d%02d%02d %02d%02d%02d\n",ttm->tm_year+1900,ttm->tm_mon+1,ttm->tm_mday,ttm->tm_hour,ttm->tm_min,ttm->tm_sec);
		swrite(NULL,u,temp);

		disconnect(u);
		return;
	}

	value=(unsigned long *)db_get_objid(arg,OB_USER);
	if (!value) {
		swrite(NULL,u,"\nThere is no invite matching that alias.\n");
		disconnect(u);
		return;
	}

	pwd=get_attribute(*value,"passwd");
	if (!pwd) {
		/* new user */
		/*
		value=(unsigned long *)db_set_newobj(arg,OB_USER);
		if (!value) {
			swrite(NULL,u,"Sorry internal error, try again later!\n");
			disconnect(u);
			return;
		}
		*/
		pwd=get_attribute(*value,"itime");
		if (!pwd) {
			swrite(NULL,u,"\nThere is no invite matching that alias.\n");
			disconnect(u);
			FREE(value);
			return;
		}
		if (time(0)-atol(pwd)>7200) {
			swrite(NULL,u,"\nYou have been invited but your invite has timed out. Sorry.\n");
			FREE(pwd);
			FREE(value);
			disconnect(u);
			return;
		}
		FREE(pwd);
		set_attribute(*value,"title","is a newbie, insult at will.");
		swrite(NULL,u,"\nNew user, Welcome!\n");
		swrite(NULL,u,(char *)config_get("prompt_newpass1"));
		echo_off(u);
		u->function=&new_passwd;
	}	
	else
	{
		swrite(NULL,u,"\n");
		swrite(NULL,u,(char *)config_get("prompt_pass"));
		echo_off(u);
		u->function=&get_passwd;
		FREE(pwd);
	}	
	u->object=*value;
	FREE(value);
}

void event(connection *u,short id, char *passed, char **ret)
{
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE];
	connection *sweep=NULL;
	char *dat=NULL;
	char *uroom=NULL;
	char *swroom=NULL;
	char *inform=NULL;
	char *beep=NULL;

	if(id==E_CONNECT)
	{
		sprintf(temp,">> %s has connected.\n",u->name);
		sprintf(temp2,">> [^YINFORM^N] %s has connected.\n",u->name);
		dat=get_attribute(u->object,"logonmsg");
		if (dat) {
			if (dat[0]!='\0') {
				sprintf(temp,">> %s %s\n",u->name,dat);
				sprintf(temp2,">> [^YINFORM^N] %s %s\n",u->name,dat);
			}
			FREE(dat);
		}
		for(sweep=users; sweep; sweep=sweep->next)
		{
			if (sweep->quiet) continue;
			if (sweep==u) continue;
			beep=get_attribute(sweep->object,"beep");
			if (beep) {
				if (in_delim(beep, u->name)) {
					swrite(NULL,sweep,"\007");
				}
				FREE(beep);
			}	
			uroom=(char *)get_user_room(u);
			swroom=(char *)get_user_room(sweep);
			/* do users in the same room */
			if (strcmp(uroom,swroom)==0) {
				swrite(NULL,sweep,temp);
			}
			else
			/* do users NOT in the same room */
			{
				inform=get_attribute(sweep->object,"inform");
				if (inform) {
					if (in_delim(inform, u->name)) {
						swrite(NULL,sweep,temp2);
					}
					FREE(inform);
				}
			}
			FREE(uroom);
			FREE(swroom);
		}
	}
	if(id==E_DISCONNECT)
	{
		sprintf(temp,"<< %s has disconnected.\n",u->name);
		sprintf(temp2,"<< [^YINFORM^N] %s has disconnected.\n",u->name);
		dat=get_attribute(u->object,"logoffmsg");
		if (dat) {
			if (dat[0]!='\0') {
				sprintf(temp,"<< %s %s\n",u->name,dat);
				sprintf(temp2,"<< [^YINFORM^N] %s %s\n",u->name,dat);
			}
			FREE(dat);
		}
		for(sweep=users; sweep; sweep=sweep->next)
		{
			if (sweep->quiet) continue;
			if (sweep==u) continue;
			uroom=(char *)get_user_room(u);
			swroom=(char *)get_user_room(sweep);
			/* do users in the same room */
			if (strcmp(uroom,swroom)==0) {
				swrite(NULL,sweep,temp);
			}
			else
			/* do users NOT in the same room */
			{
				inform=get_attribute(sweep->object,"inform");
				if (inform) {
					if (in_delim(inform, u->name)) {
						swrite(NULL,sweep,temp2);
					}
					FREE(inform);
				}
			}
			FREE(uroom);
			FREE(swroom);
		}
	}
	if(id==E_RECONNECT)
	{
		sprintf(temp,"<<>> %s has reconnected.\n",u->name);
		sprintf(temp2,"<<>> [^YINFORM^N] %s has reconnected.\n",u->name);
		for(sweep=users; sweep; sweep=sweep->next)
		{
			if (sweep->quiet) continue;
			if (sweep==u) continue;
			uroom=(char *)get_user_room(u);
			swroom=(char *)get_user_room(sweep);
			/* do users in the same room */
			if (strcmp(uroom,swroom)==0) {
				swrite(NULL,sweep,temp);
			}
			else
			/* do users NOT in the same room */
			{
				inform=get_attribute(sweep->object,"inform");
				if (inform) {
					if (in_delim(inform, u->name)) {
						swrite(NULL,sweep,temp2);
					}
					FREE(inform);
				}
			}
			FREE(uroom);
			FREE(swroom);
		}
	}
}

int init()
{
	add_command("invite",&invite,NULL,NULL);
	return 1;
}

int uninit()
{
	return 0;
}

char *version()
{
	return "Login module (c) 2004 Karl Bastiman $Revision: 2.1 $";
}
