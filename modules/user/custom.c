/* $Id: custom.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

void muffle(connection *u, char *arg)
{
	char *dat=NULL;
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE];

	if (arg) {
		if (strcasecmp(arg,"clock")==0) {
			dat=get_attribute(u->object,"muffle_clock");
			if (dat) {
				if (*dat=='Y') {
					sprintf(temp,"N");
				}
				else
				{
					sprintf(temp,"Y");
				}
				FREE(dat);
			}
			else
			{
				sprintf(temp,"Y");
			}
			set_attribute(u->object,"muffle_clock",temp);
			sprintf(temp2,"You will %s see the clock each hour.\n",*temp!='Y'?"now":"no longer");
			swrite(NULL,u,temp2);
			return;
		}	
		if (strcasecmp(arg,"news")==0) {
			dat=get_attribute(u->object,"muffle_news");
			if (dat) {
				if (*dat=='Y') {
					sprintf(temp,"N");
				}
				else
				{
					sprintf(temp,"Y");
				}
				FREE(dat);
			}
			else
			{
				sprintf(temp,"Y");
			}
			set_attribute(u->object,"muffle_news",temp);
			sprintf(temp2,"You will %s see the news alerts.\n",*temp!='Y'?"now":"no longer");
			swrite(NULL,u,temp2);
			return;
		}	
		if (strcasecmp(arg,"radio")==0) {
			dat=get_attribute(u->object,"muffle_radio");
			if (dat) {
				if (*dat=='Y') {
					sprintf(temp,"N");
				}
				else
				{
					sprintf(temp,"Y");
				}
				FREE(dat);
			}
			else
			{
				sprintf(temp,"Y");
			}
			set_attribute(u->object,"muffle_radio",temp);
			sprintf(temp2,"You will %s see radio messages.\n",*temp=='N'?"now":"no longer");
			swrite(NULL,u,temp2);
			return;
		}	
		if (strcasecmp(arg,"bingo")==0) {
			dat=get_attribute(u->object,"muffle_bingo");
			if (dat) {
				if (*dat=='Y') {
					sprintf(temp,"N");
				}
				else
				{
					sprintf(temp,"Y");
				}
				FREE(dat);
			}
			else
			{
				sprintf(temp,"Y");
			}
			set_attribute(u->object,"muffle_bingo",temp);
			sprintf(temp2,"You will %s see bingo messages.\n",*temp=='N'?"now":"no longer");
			swrite(NULL,u,temp2);
			return;
		}	
		if (strcasecmp(arg,"idle")==0) {
			dat=get_attribute(u->object,"muffle_idle");
			if (dat) {
				if (*dat=='Y') {
					sprintf(temp,"N");
				}
				else
				{
					sprintf(temp,"Y");
				}
				FREE(dat);
			}
			else
			{
				sprintf(temp,"Y");
			}
			set_attribute(u->object,"muffle_idle",temp);
			sprintf(temp2,"You and others will %s be informed of when you unidle.\n",*temp=='N'?"now":"no longer");
			swrite(NULL,u,temp2);
			return;
		}	
		if (strcasecmp(arg,"cname")==0) {
			dat=get_attribute(u->object,"muffle_cname");
			if (dat) {
				if (*dat=='Y') {
					sprintf(temp,"N");
				}
				else
				{
					sprintf(temp,"Y");
				}
				FREE(dat);
			}
			else
			{
				/* default behaviour for cname is to see it
				if it is not currently set */
				sprintf(temp,"N");
			}
			set_attribute(u->object,"muffle_cname",temp);
			sprintf(temp2,"You will %s see coloured names.\n",*temp=='Y'?"now":"no longer");
			swrite(NULL,u,temp2);
			return;
		}	
	}
	swrite(NULL,u,"Dunno how to muffle that. (see ^Ghelp muffle^N)\n");
}

void unlock(connection *u, char *arg)
{
	char *dat=NULL;

	dat=get_attribute(u->object,"passwd");

	if (!dat) {
		/* fucked up somehow, this should never happen */
		u->function=&run_command;
		echo_on(u);
		return;
	}

	if (strcmp(arg,dat)==0) {
		u->function=&run_command;
		echo_on(u);
		swrite(NULL,u,"Session ^GUNLOCKED^N...\n");
		FREE(dat);
		return;
	}

	FREE(dat);
	swrite(NULL,u,"-- This session is now ^RLOCKED^N type password to unlock --\n");
}

void xlock(connection *u, char *arg)
{
	swrite(NULL,u,"-- This session is now ^RLOCKED^N type password to unlock --\n");
	u->function=&unlock;
	echo_off(u);
}

void xprompt(connection *u, char *arg)
{
	if (!arg) {
		swrite(NULL,u,"Prompt turned off.\n");
		set_attribute(u->object,"prompt","");
		return;
	}

	if (strlen(arg)>30) {
		swrite(NULL,u,"Prompts need to be less than 30 characters.\n");
		return;
	}

	swrite(NULL,u,"Prompt set.\n");
	set_attribute(u->object,"prompt",arg);
}

void idlemsg(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Specify an emote for an idlemsg.\n");
		return;
	}

	if (strlen(arg)>(48-strlen(u->name))) {
		swrite(NULL,u,"Too long.\n");
		return;
	}

	set_attribute(u->object,"idlemsg",arg);
	sprintf(temp,"Idlemsg set to ...\n%s %s\n",u->name,arg);
	swrite(NULL,u,temp);
}	

void emailsee(connection *u, char *arg)
{
	if (!arg) {
		swrite(NULL,u,"Set to [P]rivate, [F]riends, [A]ll\n");
		return;
	}

	switch (arg[0]) {
		case 'p':
		case 'P': swrite(NULL,u,"Set to Private.\n"); break;
		case 'f':
		case 'F': swrite(NULL,u,"Set to Friends only.\n"); break;
		case 'a':
		case 'A': swrite(NULL,u,"Set to Public.\n"); break;
		default : swrite(NULL,u,"Unknown setting.\n"); return; break;
	}

	set_attribute(u->object,"emailsee",arg);
}	
		
void email(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Provide an email address.\n");
		return;
	}

	sprintf(temp, "Email set to %s.\n", arg);
	set_attribute(u->object,"email",arg);
	swrite(NULL,u,temp);
}

void jabber(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Provide an jabber address.\n");
		return;
	}

	sprintf(temp, "Jabber set to %s.\n", arg);
	set_attribute(u->object,"jabber",arg);
	swrite(NULL,u,temp);
}

void logonmsg(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		set_attribute(u->object,"logonmsg","");
		swrite(NULL,u,"Logonmsg cleared.\n");
		return;
	}

	if (strlen(arg)>55) {
		swrite(NULL,u,"Too long.\n");
		return;
	}

	set_attribute(u->object,"logonmsg",arg);
	sprintf(temp,"Logonmsg set to...\n  %s %s\n",u->name,arg);
	swrite(NULL,u,temp);
}

void logoffmsg(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		set_attribute(u->object,"logoffmsg","");
		swrite(NULL,u,"Logoffmsg cleared.\n");
		return;
	}

	if (strlen(arg)>55) {
		swrite(NULL,u,"Too long.\n");
		return;
	}

	set_attribute(u->object,"logoffmsg",arg);
	sprintf(temp,"Logoffmsg set to...\n  %s %s\n",u->name,arg);
	swrite(NULL,u,temp);
}

void colour(connection *u, char *arg)
{
	char *dat=NULL;

	dat=get_attribute(u->object,"colour");

	if (dat) {
		if (dat[0]=='N') {
			swrite(NULL,u,"You turn ^RC^Bo^Ml^Yo^Cu^Mr^N On!\n");
			set_attribute(u->object,"colour","Y");
			FREE(dat);
			return;
		}
		FREE(dat);
	}

	swrite(NULL,u,"You turn colour OFF!\n");
	set_attribute(u->object,"colour","N");
}

void hitells(connection *u, char *arg)
{
	char *dat=NULL;

	dat=get_attribute(u->object,"hitells");

	if (dat) {
		if (dat[0]=='N') {
			swrite(NULL,u,"You turn ^HHitells^N On!\n");
			set_attribute(u->object,"hitells","Y");
			FREE(dat);
			return;
		}
		FREE(dat);
	}

	swrite(NULL,u,"You turn Hitells OFF!\n");
	set_attribute(u->object,"hitells","N");
}

void cname(connection *u, char *arg)
{
	char *seek=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Use letters to signify a cname for instance for ^RK^Been^Yy^N:\n  You would do 'cname RBBBY'.\n");
		return;
	}

	if (strlen(arg)!=strlen(u->name)) {
		swrite(NULL,u,"Use letters to signify a cname for instance for ^RK^Been^Yy^N:\n  You would do 'cname RBBBY'.\n");
		return;
	}

	for (seek=arg;*seek!='\0';seek++) {
		if (!iscolour(*seek)) {
			sprintf(temp,"Invalid colour code '%c'\n",*seek);
			swrite(NULL,u,temp);
			return;
		}
	}	

	set_attribute(u->object,"cname",arg);
	swrite(NULL,u,"cname set.\n");
}	
		
void recap(connection *u, char *arg)
{
	if (!arg) {
		swrite(NULL,u,"Specify your name in the capitalisation you want.\n");
		return;
	}

	if (strlen(arg)!=strlen(u->name)) {
		swrite(NULL,u,"Specify your name in the capitalisation you want.\n");
		return;
	}

	if (strcasecmp(arg,u->name)!=0) {
		swrite(NULL,u,"Trying to change your name eh?\n");
		return;
	}

	set_attribute(u->object,"recap",arg);
	swrite(NULL,u,"Recap set.\n");
}	
		
int init()
{
	add_command("logonmsg",&logonmsg,NULL,NULL);
	add_command("logoffmsg",&logoffmsg,NULL,NULL);
	add_command("hitells",&hitells,NULL,NULL);
	add_command("color",&colour,NULL,NULL);
	add_command("colour",NULL,"color",NULL);
	add_command("cname",&cname,NULL,NULL);
	add_command("recap",&recap,NULL,NULL);
	add_command("email",&email,NULL,NULL);
	add_command("jabber",&jabber,NULL,NULL);
	add_command("emailsee",&emailsee,NULL,NULL);
	add_command("idlemsg",&idlemsg,NULL,NULL);
	add_command("prompt",&xprompt,NULL,NULL);
	add_command("lock",&xlock,NULL,NULL);
	add_command("muffle",&muffle,NULL,NULL);
	return 1;
}


int uninit()
{
	remove_command("logonmsg",NULL);
	remove_command("logoffmsg",NULL);
	remove_command("hitells",NULL);
	remove_command("colour",NULL);
	remove_command("color",NULL);
	remove_command("cname",NULL);
	remove_command("recap",NULL);
	remove_command("email",NULL);
	remove_command("jabber",NULL);
	remove_command("emailsee",NULL);
	remove_command("idlemsg",NULL);
	remove_command("prompt",NULL);
	remove_command("lock",NULL);
	remove_command("muffle",NULL);
	return 1;
}	

char *version()
{	
	return "Player customization (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
