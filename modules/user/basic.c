/* $Id: basic.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"
#include "basic.h"

#define PINGDELAY	120	/* in seconds */

extern void check_cache();
extern void wsock(int ,char *);

struct multi_st *multis=NULL;

time_t pingtimer=0;

/* this is staying in the module because i dont see it being used elsewhere
*/
connection *getubyname_notself(connection *u, char *name)
{
	connection *sweep=NULL;

	if (!name) return NULL;
	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		if (sweep==u) continue;
		if (strncasecmp(sweep->name,name,strlen(name))==0) {
			return sweep;
		}
	}

	return NULL;
}

void timeout_multi()
{
	struct multi_st *sweep=NULL;
	struct multi_st *next=NULL;
	struct multi_st *last=NULL;

	for (sweep=multis;sweep;sweep=next) {
		next=sweep->next;
		if (time(0)-sweep->est>1800) {
			if (last) {
				last->next=next;
			}
			else
			{
				multis=next;
			}	
			FREE(sweep->names);
			FREE(sweep);
		}
		else
		{
			last=sweep;
		}	
	}	
}

int create_multi(connection *u, char *names)
{
	struct multi_st *sweep=NULL;
	int idx=1;
	int fnd=0;
	
	for (idx=1;;idx++) {
		fnd=0;
		for (sweep=multis;sweep;sweep=sweep->next) {
			if (sweep->index==idx) {
				fnd=1;
			}
		}
		if (!fnd) break;
	}	

	if (!multis) {
		multis=MALLOC(sizeof(struct multi_st),"create_multi:multis");
		sweep=multis;
	}
	else
	{
		for (sweep=multis; sweep->next; sweep=sweep->next);

		sweep->next=MALLOC(sizeof(struct multi_st),"create_multi:sweep->next");
		sweep=sweep->next;
	}

	sweep->next=NULL;
	sweep->est=time(0);
	sweep->index=idx;
	sweep->names=MALLOC(strlen(names)+strlen(u->name)+6,"create_multi:sweep->names");
	sprintf(sweep->names,"%s,%s",names,u->name);
	
	return idx;
}

char *get_multi(connection *c, int index)
{
	struct multi_st *sweep=NULL;

	for (sweep=multis; sweep; sweep=sweep->next) {
		if (sweep->index==index) {
			if (in_delim(sweep->names, c->name)) {
				return sweep->names;
			}	
		}	
	}

	return NULL;
}


/* returns 1 on true, 0 for no match */
int compare_multis(char *comp1, char *comp2)
{
	char *ele1, *ele2;
	char *moo1, *moo2;
	int fnd=0, ret=1;
	char *comp3=NULL;

	if (strlen(comp1)!=strlen(comp2)) return 0;

	comp3=MALLOC(strlen(comp2)+2,"compare_multis:comp3");
	for (ele1=strtok_r(comp1,",",&moo1); ele1; ele1=strtok_r(NULL,",",&moo1)) {
		fnd=0;
		moo2=NULL;
		strcpy(comp3,comp2);
		for (ele2=strtok_r(comp3,",",&moo2); ele2; ele2=strtok_r(NULL,",",&moo2)) {
			if (strcasecmp(ele1,ele2)==0) {
				fnd=1;
			}
		}
		if (!fnd) ret=0;
	}	

	FREE(comp3);
	
	return ret;
}

int is_multi(connection *u, char *whoto)
{
	struct multi_st *sweep=NULL;
	char *comp=NULL;
	char *comp2=NULL;

	comp=MALLOC(strlen(whoto)+strlen(u->name)+6,"is_multi:comp");

	sprintf(comp,"%s,%s",whoto,u->name);

	for (sweep=multis; sweep; sweep=sweep->next) {
		comp2=MALLOC(strlen(sweep->names)+2,"is_multi:comp2");
		strcpy(comp2,sweep->names);
		if (compare_multis(comp,comp2)) {
			sweep->est=time(0);
			return sweep->index;
		}
		FREE(comp2);
	}

	FREE(comp);
	return 0;
}

/* is who ignoring ignoring?
*/
int isignore(connection *who, connection *ignoring) 
{
	char *dat=NULL;

	dat=get_attribute(who->object, "ignores");

	if (!dat) return 0;

	if (in_delim(dat, ignoring->name)) {
		FREE(dat);
		return 1;
	}

	FREE(dat);
	return 0;
}

void review(connection *u, char *arg)
{
	char temp[TMP_SIZE];
	int count=0;

	format_bar(u, "Your tell history",temp);
	swrite(NULL,u,temp);

	for (count=0;count<10;count++) {
		if (u->tellbuf[count]) {
			swrite(NULL,u,u->tellbuf[count]);
		}	
	}

	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
}

/* this is not a command related function 
**
** element 0 is oldest
*/
void add_review(connection *u, char *arg)
{
	int count=0;

	if (!arg) return;  /* unlikely */
	if (!u->tellbuf[9]) {
		for (count=0;u->tellbuf[count];count++); 
	}
	else
	{
		FREE(u->tellbuf[0]);
		for (count=0;count<9;count++) {
			u->tellbuf[count]=u->tellbuf[count+1];
		}
		count=9;
	}

	u->tellbuf[count]=(char *)MALLOC(strlen(arg)+2,"add_review->tellbuf");
	strcpy(u->tellbuf[count],arg);
}

void xtell(connection *u, char *arg)
{
	char *what=NULL;
	char *whoto=NULL;
	char *moo=NULL;
	char *ele=NULL;
	char *told=NULL;
	char *told2=NULL;
	char *multi=NULL;
	int mulno=0;
	int numtold=0;
	connection *u2=NULL;
	char temp[TMP_SIZE];
	char buftemp[TMP_SIZE];
	char temp2[TMP_SIZE];
	char *im=NULL;

	buftemp[0]='\0';

	if (!arg) {
		swrite(NULL,u,"Tell who, what?\n");
		return;
	}

	/* right....
	**
	** we need [tell] user,user,... blah
	*/

	/* first look to see if it is a new multi */

	what=strchr(arg,' ');
	if (!what) {
		swrite(NULL,u,"Tell what?\n");
		return;
	}
	*what='\0';
	what++;

	mulno=atoi(arg); /* returns 0 if it aint a number */
	multi=get_multi(u,mulno);

	if ((mulno>0)&&(!multi)) {
		swrite(NULL,u,"No such multi.\n");
		return;
	}	

	whoto=MALLOC(strlen(multi?multi:arg)+4,"tell:whoto");
	strcpy(whoto,multi?multi:arg);


	told=MALLOC(strlen(whoto)*2,"tell:told");
	told[0]='\0';
	told2=MALLOC(strlen(whoto)*2,"tell:told2");
	told2[0]='\0';
	/* arg should now contain the name(s) to tell */
	
	/* I wanted to avoid this, but the told list is silly if we
	** just use arg, so be it...
	*/
	for (ele=strtok_r(whoto,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		u2=getubyname(u,ele,temp);
		if (!u2) {
			swrite(NULL,u,temp);
			continue;
		}
		if (u2==u) {
			if (mulno<1) {
				/* try to see if they mean another user */
				u2=getubyname_notself(u,ele);
				if (!u2) {
					swrite(NULL,u,"Talking to yourself? bit odd...\n");
					continue; /* dont tell oneself */
				}	
			}	
			else
			{
				continue;
			}	
		}	
		if (isignore(u2,u)) {
			sprintf(temp," %s is ignoring you\n",u2->name);
			swrite(NULL,u,temp);
			continue;
		}	
		if (isignore(u,u2)) {
			sprintf(temp," you are ignoring %s, and cannot tell them anything.\n",u2->name);
			swrite(NULL,u,temp);
			continue;
		}	
		if (told[0]!='\0') strcat(told,", ");
		strcat(told,u2->name);
		if (told2[0]!='\0') strcat(told2,",");
		strcat(told2,u2->name);
		numtold++;
	}

	if (mulno==0&&numtold>1) {
		mulno=is_multi(u,told2);
		if (!mulno) {
			mulno=create_multi(u, told2);
		}	
	}

	if (mulno) {
		sprintf(temp2,"(%d) ",mulno);
	}
	else
	{
		temp2[0]='\0';
	}

	strcpy(whoto,multi?multi:arg);
	moo=NULL;
	for (ele=strtok_r(told2,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		u2=getubyname(u,ele,temp);
		if (!u2) continue;
		if (u2==u) continue; /* dont tell oneself */
		if (isignore(u2,u)) continue;
		if (isignore(u,u2)) continue;
		switch (arg[strlen(arg)-1])
		{
			case '!':
				sprintf(temp,"* %s%s exclaims to %s '^*%s^N'\n",temp2,u->name,(!strchr(told,','))?"you":told,what);
				break;
			case '?':
				sprintf(temp,"* %s%s asks %s '^*%s^N'\n",temp2,u->name,(!strchr(told,','))?"you":told,what);
				break;
			default:
				sprintf(temp,"* %s%s tells %s '^*%s^N'\n",temp2,u->name,(!strchr(told,','))?"you":told,what);
				break;
		}
		strcpy(buftemp,temp);
		swrite(u,u2,temp);
	}

	if (told[0]!='\0') {
		switch (arg[strlen(arg)-1])
		{
			case '!':
				sprintf(temp,"%sYou exclaim to %s '^*%s^N'\n",temp2,told,what);
				break;
			case '?':
				sprintf(temp,"%sYou ask %s '^*%s^N'\n",temp2,told,what);
				break;
			default:
				sprintf(temp,"%sYou tell to %s '^*%s^N'\n",temp2,told,what);
				break;
		}
		swrite(NULL,u,temp);
	}	

	if ((!strchr(told,','))&&(u2)) {
		im=get_attribute(u2->object,"idlemsg");
		if (im) {
			if (im[0]!='\0') {
				sprintf(temp,"%s is IDLE:\n  %s %s\n",u2->name,u2->name,im);
				swrite(NULL,u,temp);
			}
			FREE(im);
		}
		add_review(u2,buftemp);
	}	

	FREE(whoto);
	FREE(told);
	FREE(told2);
}

/* more or less the same as tell, there is no doubt a generic way of doing this
** but i cant be bothered, and tell is complicated enough */
void remote(connection *u, char *arg)
{
	char *what=NULL;
	char *whoto=NULL;
	char *moo=NULL;
	char *ele=NULL;
	char *told=NULL;
	char *told2=NULL;
	char *multi=NULL;
	int mulno=0;
	int numtold=0;
	connection *u2=NULL;
	char temp[TMP_SIZE];
	char buftemp[TMP_SIZE];
	char temp2[TMP_SIZE];
	char *im=NULL;

	buftemp[0]='\0';

	if (!arg) {
		swrite(NULL,u,"Remote what to who?\n");
		return;
	}

	/* right....
	**
	** we need [tell] user,user,... blah
	*/

	/* first look to see if it is a new multi */

	what=strchr(arg,' ');
	if (!what) {
		swrite(NULL,u,"Remote what?\n");
		return;
	}
	*what='\0';
	what++;

	mulno=atoi(arg); /* returns 0 if it aint a number */
	multi=get_multi(u,mulno);

	if ((mulno>0)&&(!multi)) {
		swrite(NULL,u,"No such multi.\n");
		return;
	}	

	whoto=MALLOC(strlen(multi?multi:arg)+4,"remote:whoto");
	strcpy(whoto,multi?multi:arg);


	told=MALLOC(strlen(whoto)*2,"remote:told");
	told[0]='\0';
	told2=MALLOC(strlen(whoto)*2,"remote:told2");
	told2[0]='\0';
	/* arg should now contain the name(s) to tell */
	
	/* I wanted to avoid this, but the told list is silly if we
	** just use arg, so be it...
	*/
	for (ele=strtok_r(whoto,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		u2=getubyname(u,ele,temp);
		if (!u2) {
			swrite(NULL,u,temp);
			continue;
		}
		if (u2==u) {
			if (mulno<1) {
				/* try to see if they mean another user */
				u2=getubyname_notself(u,ele);
				if (!u2) {
					swrite(NULL,u,"Talking to yourself? bit odd...\n");
					continue; /* dont tell oneself */
				}	
			}	
			else
			{
				continue;
			}	
		}	
		if (isignore(u2,u)) {
			sprintf(temp," %s is ignoring you\n",u2->name);
			swrite(NULL,u,temp);
			continue;
		}	
		if (isignore(u,u2)) {
			sprintf(temp," you are ignoring %s, and cannot remote them anything.\n",u2->name);
			swrite(NULL,u,temp);
			continue;
		}	
		if (told[0]!='\0') strcat(told,", ");
		strcat(told,u2->name);
		if (told2[0]!='\0') strcat(told2,",");
		strcat(told2,u2->name);
		numtold++;
	}

	if (mulno==0&&numtold>1) {
		mulno=is_multi(u,told2);
		if (!mulno) {
			mulno=create_multi(u, told2);
		}	
	}

	if (mulno) {
		sprintf(temp2,"(%d) ",mulno);
	}
	else
	{
		temp2[0]='\0';
	}

	strcpy(whoto,multi?multi:arg);
	moo=NULL;
	for (ele=strtok_r(told2,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		u2=getubyname(u,ele,temp);
		if (!u2) continue;
		if (u2==u) continue; /* dont tell oneself */
		if (isignore(u2,u)) continue;
		if (isignore(u,u2)) continue;
		sprintf(temp,"* %s%s ^*%s ^N(to %s^N)\n",temp2,u->name,what,(!strchr(told,','))?"you":told);
		swrite(u,u2,temp);
		strcpy(buftemp,temp);
	}

	if (told[0]!='\0') {
		sprintf(temp,"%sYou emote: %s ^*%s ^N(to %s^N)\n",temp2,u->name,what,told);
		swrite(NULL,u,temp);
	}	

	if ((!strchr(told,','))&&(u2)) {
		im=get_attribute(u2->object,"idlemsg");
		if (im) {
			if (im[0]!='\0') {
				sprintf(temp,"%s is IDLE:\n  %s %s\n",u2->name,u2->name,im);
				swrite(NULL,u,temp);
			}
			FREE(im);
		}
		add_review(u2,buftemp);
	}	

	FREE(whoto);
	FREE(told);
	FREE(told2);
}

void emote(connection *u,char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Emote what?\n");
		return;
	}	
	sprintf(temp,"%s%s%s\n",u->name,(arg[0]=='\''||arg[0]==',')?"":" ",arg);
	sroom(u,temp);
	sprintf(temp," You emote: %s%s%s\n",u->name,(arg[0]=='\''||arg[0]==',')?"":" ",arg);
	swrite(NULL,u,temp);
}

void pemote(connection *u,char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Emote what?\n");
		return;
	}	
	sprintf(temp,"%s's %s\n",u->name,arg);
	sroom(u,temp);
	sprintf(temp," You emote: %s's %s\n",u->name,arg);
	swrite(NULL,u,temp);
}

void say(connection *u,char *arg)
{
	char temp[TMP_SIZE];

	debug("say");
	if (!arg) {
		swrite(NULL,u,"Say what?\n");
		return;
	}	
	switch (arg[strlen(arg)-1])
	{
		case '!':
			sprintf(temp,"%s exclaims '%s^N'\n",u->name,arg);
			break;
		case '?':
			sprintf(temp,"%s asks '%s^N'\n",u->name,arg);
			break;
		default:
			sprintf(temp,"%s says '%s^N'\n",u->name,arg);
			break;
	}
	sroom(u,temp);
	switch (arg[strlen(arg)-1])
	{
		case '!':
			sprintf(temp," You exclaim '%s^N'\n",arg);
			break;
		case '?':
			sprintf(temp," You ask '%s^N'\n",arg);
			break;
		default:
			sprintf(temp," You say '%s^N'\n",arg);
			break;
	}

	swrite(NULL,u,temp);
	/* check_cache();  */
	debug("say--");
}

void sing(connection *u,char *arg)
{
	char temp[TMP_SIZE];
	if (!arg) {
		swrite(NULL,u,"Sing what?\n");
		return;
	}	
	sprintf(temp,"%s sings o/~ %s ^No/~\n",u->name,arg);
	sroom(u,temp);
	sprintf(temp," You sing o/~ %s ^No/~\n",arg);
	swrite(NULL,u,temp);
}

void think(connection *u,char *arg)
{
	char temp[TMP_SIZE];
	if (!arg) {
		swrite(NULL,u,"Think what?\n");
		return;
	}	
	sprintf(temp,"%s thinks . o O ( %s ^N)\n",u->name,arg);
	sroom(u,temp);
	sprintf(temp," You think . o O ( %s ^N)\n",arg);
	swrite(NULL,u,temp);
}

void quote(connection *u,char *arg)
{
	char temp[TMP_SIZE];
	if (!arg) {
		swrite(NULL,u,"Quote what?\n");
		return;
	}	
	sprintf(temp,"%s ^Yquotes -[^N %s ^Y]-^N\n",u->name,arg);
	sroom(u,temp);
	sprintf(temp," You ^Yquote -[^N %s ^Y]-^N\n",arg);
	swrite(NULL,u,temp);
}

void quit(connection *u,char *arg)
{
	raise_event(u,E_DISCONNECT,NULL,NULL);
	disconnect(u);
}

void who(connection *u,char *arg)
{
	connection *sweep;
	char f[1024];
	int i;
	char temp[TMP_SIZE];
	char *title=NULL;
	unsigned long *objid=NULL;

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
		objid=db_get_objid(sweep->name,OB_USER);
		if (objid) {
			/* if we dont get here it's fucked anyway */
			title=get_attribute(*objid,"title");
			if (!title) {
				title=MALLOC(2*sizeof(char),"who:title");
				title[0]='\0';
			}	
			FREE(objid);
		}	
		sprintf(temp,"%s%s%s\n",sweep->name,*title==','||*title=='\''?"":" ",title);
		swrite(NULL,u,temp);
		FREE(title);
	}
	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
}

void age(connection *u, char *arg)
{
	int iage=0;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Well, how old are you?\n");
		return;
	}
	
	iage=atoi(arg);

	if (iage<1||iage>150) {
		swrite(NULL,u,"Not a valid age.\n");
		return;
	}

	sprintf(temp,"%d",iage);
	set_attribute(u->object,"age",temp);
	sprintf(temp,"Your age is set to %d.\n",iage);
	swrite(NULL,u,temp);
}

/* M,m,F,f,N,n */
void gender(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Syntax: gender M/F/N\n");
		return;
	}
	
	switch (arg[0]) {
		case 'M':
		case 'm': strcpy(temp,"M"); break;
		case 'F':
		case 'f': strcpy(temp,"F"); break;
		case 'N':
		case 'n': strcpy(temp,"N"); break;
		default: swrite(NULL,u,"Syntax: gender M?F?N\n");
			 return; break;
	}

	set_attribute(u->object,"gender",temp);
	sprintf(temp,"Your gender is set to %s.\n",getgen(u,"Male","Female","Neuter"));
	swrite(NULL,u,temp);
}

void title(connection *u,char *arg)
{
	unsigned long *objid=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Title reset.\n");
	}
	else
	{
		if (strlen(arg)>(73-strlen(u->name))) {
			swrite(NULL,u,"Title too long.\n");
			return;
		}

		swrite(NULL,u,"Title set to:\n");
		sprintf(temp,"%s %s\n",u->name,arg);
		swrite(NULL,u,temp);
	}

	objid=db_get_objid(u->name,OB_USER);
	if (objid) {
		/* if we dont get here it's fucked anyway */
		set_attribute(*objid,"title",arg?arg:"");
		FREE(objid);
	}	
}

/*
void prefer(connection *u, char *arg)
{
	if (!arg) {
		swrite(NULL,u,"prefer who?");
		return;
	}


}
*/

void wake(connection *u, char *arg)
{
	char *dat=NULL;
	connection *target=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Who do you want to wake up?\n");
		return;
	}

	target=getubyname(u,arg,temp);

	if (!target) {
		swrite(NULL,u,temp);
		return;
	}

	if (target==u) {
		swrite(NULL,u,"Need to wake up? Get some coffee.\n");
		return;
	}
	
	dat=get_attribute(u->object,"ignores");
	if (dat) {
		if (in_delim(dat,target->name)) {
			swrite(NULL,u,"You are ignoring that user.\n");
			FREE(dat);
			return;
		}
		FREE(dat);
	}

	dat=get_attribute(target->object,"ignores");
	if (dat) {
		if (in_delim(dat,u->name)) {
			swrite(NULL,u,"That user is currently ignoring you.\n");
			FREE(dat);
			return;
		}
		FREE(dat);
	}

	sprintf(temp,"You send a wake up call to %s.\n",target->name);
	swrite(NULL,u,temp);
	sprintf(temp,"\007^B!!!!^N %s tries to wake you up ^B!!!!^N\n",u->name);
	swrite(NULL,target,temp);
}		

void cls(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	sprintf(temp,"\033[1;1H\033[2J");

	wsock(u->fd,temp);
}

void repeat(connection *u, char *arg)
{
	char arg2[TMP_SIZE];
	char *ccat=NULL;
	command *this=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a user to redirect the last command to.\n");
		return;
	}

	if (!u->prev_command) {
		swrite(NULL,u,"There is no command you can repeat yet.\n");
		return;
	}

	if (u->lastarg) ccat=strchr(u->lastarg,' ');
	if (strlen(ccat?ccat:"")+strlen(arg) > TMP_SIZE-1) {
		swrite(NULL,u,"Sorry cannot repeat the last command.\n");
		return;
	}

	this=(command *)u->prev_command;
	if (get_command_flag(this->name,C_REPEAT)<1) {
		swrite(NULL,u,"Your last command is not compatible with repeat.\n");
		return;
	}

	sprintf(arg2,"%s%s",arg,ccat?ccat:"");
	this->function(u,arg2);
}

void xecho(connection *u, char *arg)
{
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Echo what?\n");
		return;
	}

	sprintf(temp, "^G+^N ^A[%s^A]^N %s\n",u->name,arg);
	sroom(u,temp);
	/* just incase tempc hanges */
	sprintf(temp, "^G+^N ^A[%s^A]^N %s\n",u->name,arg);
	swrite(NULL,u,temp);
}

void xpasswd(connection *u, char *arg)
{
	if (!arg) {
		swrite(NULL,u,"usage: passwd [new password]\n");
		return;
	}

	if (strlen(arg)<3||strlen(arg)>15) {
		swrite(NULL,u,"Passwords must be between 3 and 15 characters.\n");
		return;
	}

	set_attribute(u->object,"passwd",arg);
	swrite(NULL,u,"  ---- Password Changed ----\n");
}

int init()
{
	add_command("<",NULL,"remote",NULL);
	add_command(",",NULL,"remote",NULL);
	add_command(">;",NULL,"remote",NULL);
	add_command(">:",NULL,"remote",NULL);
	add_command(">",NULL,"tell",NULL);
	add_command(".",NULL,"tell",NULL);
	add_command("'",NULL,"say",NULL);
	add_command("`",NULL,"say",NULL);
	add_command(";;",NULL,"pemote",NULL);
	add_command("::",NULL,"pemote",NULL);
	add_command(";",NULL,"emote",NULL);
	add_command(":",NULL,"emote",NULL);
	add_command(")",NULL,"sing",NULL);
	add_command("~",NULL,"think",NULL);
	add_command("+",NULL,"echo",NULL);
	add_command("!!",NULL,"repeat",NULL);
	add_command("history",NULL,"review",NULL);
	add_command("remote",&remote,NULL,NULL);
	set_command_flag("remote",C_REPEAT);
	add_command("tell",&xtell,NULL,NULL);
	set_command_flag("tell",C_REPEAT);
	add_command("say",&say,NULL,NULL);
	add_command("quit",&quit,NULL,NULL);
	add_command("who",&who,NULL,NULL);
	add_command("commands",&listcommands,NULL,NULL);
	add_command("emote",&emote,NULL,NULL);
	add_command("pemote",&pemote,NULL,NULL);
	add_command("sing",&sing,NULL,NULL);
	add_command("think",&think,NULL,NULL);
	add_command("title",&title,NULL,NULL);
	add_command("age",&age,NULL,NULL);
	add_command("gender",&gender,NULL,NULL);
	add_command("wake",&wake,NULL,NULL);
	set_command_flag("wake",C_REPEAT);
	add_command("quote",&quote,NULL,NULL);
	add_command("cls",&cls,NULL,NULL);
	add_command("repeat",&repeat,NULL,NULL);
	add_command("review",&review,NULL,NULL);
	add_command("echo",&xecho,NULL,NULL);
	add_command("passwd",&xpasswd,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("wake",NULL);
	remove_command("<",NULL);
	remove_command(">;",NULL);
	remove_command(">:",NULL);
	remove_command(",",NULL);
	remove_command("remote",NULL);
	remove_command(">",NULL);
	remove_command(".",NULL);
	remove_command("tell",NULL);
	remove_command("say",NULL);
	remove_command("'",NULL);
	remove_command("`",NULL);
	remove_command("+",NULL);
	remove_command("!!",NULL);
	remove_command("quit",NULL);
	remove_command("who",NULL);
	remove_command("commands",NULL);
	remove_command("emote",NULL);
	remove_command("pemote",NULL);
	remove_command("::",NULL);
	remove_command(";;",NULL);
	remove_command(";",NULL);
	remove_command(":",NULL);
	remove_command("sing",NULL);
	remove_command(")",NULL);
	remove_command("think",NULL);
	remove_command("~",NULL);
	remove_command("title",NULL);
	remove_command("age",NULL);
	remove_command("gender",NULL);
	remove_command("quote",NULL);
	remove_command("cls",NULL);
	remove_command("repeat",NULL);
	remove_command("review",NULL);
	remove_command("history",NULL);
	remove_command("echo",NULL);
	remove_command("passwd",NULL);
	return 1;
}

void tick()
{
	connection *sweep=NULL;

	timeout_multi();

	if (time(0)-pingtimer<PINGDELAY) return;
	pingtimer=time(0);
	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		ping(sweep);
	}
}	

char *version()
{
	return "Basic Commands (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
