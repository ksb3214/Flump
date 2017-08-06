/* $Id: lists.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

connection *get_prefer(connection *u, char *name)
{
	char *prefer=NULL;
	char *buf=NULL;
	char *ele=NULL;
        char *moo=NULL;
	connection *tmpu=NULL;

	prefer=get_attribute(u->object, "prefer");

	if (!prefer) return NULL;

	buf=(char *)MALLOC(strlen(prefer)+2, "get_prefer:buf");
	strcpy(buf,prefer);

        for (ele=strtok_r(buf,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
        {
		if (strncasecmp(ele,name,strlen(name))==0) {
			for (tmpu=users;tmpu;tmpu=tmpu->next) {
				if (tmpu->quiet) continue;
				if (strcasecmp(tmpu->name,ele)==0) {
					FREE(buf);
					FREE(prefer);
					return tmpu;
				}
			}
		}	
        }

	FREE(buf);
	FREE(prefer);

	return NULL;
}

void prefer(connection *u, char *arg)
{
	char *lst=NULL;
	unsigned long *oid=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a user [fullname] to be preferred for communication.\n");
		return;
	}

	lst=get_attribute(u->object,"prefer");

	if (!lst) {
		lst=MALLOC(2,"prefer:lst");
		lst[0]='\0';
	}

	if (in_delim(lst,arg)) {
		/* remove */
		remove_from(&lst,arg);
		set_attribute(u->object,"prefer",lst);
		sprintf(temp,"%s removed from prefer list.\n",arg);
		swrite(NULL,u,temp);
	}
	else
	{
		/* add */
		oid=db_get_objid(arg,OB_USER);
		if (!oid) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		dat=get_attribute(*oid,"passwd");
		if (!dat) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		FREE(dat);
		if (*oid==u->object) {
			swrite(NULL,u,"You don't quite get this do you?\n");
			FREE(oid);
			return;
		}

		insert_into(&lst,arg);
		set_attribute(u->object,"prefer",lst);
		sprintf(temp,"You will now prefer %s for comunication.\n",arg);
		swrite(NULL,u,temp);
		FREE(oid);
	}

	FREE(lst);
}

void inform(connection *u, char *arg)
{
	char *lst=NULL;
	unsigned long *oid=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a user [fullname] to be informed of when they login.\n");
		return;
	}

	lst=get_attribute(u->object,"inform");

	if (!lst) {
		lst=MALLOC(2,"inform:lst");
		lst[0]='\0';
	}

	if (in_delim(lst,arg)) {
		/* remove */
		remove_from(&lst,arg);
		set_attribute(u->object,"inform",lst);
		sprintf(temp,"%s removed from inform list.\n",arg);
		swrite(NULL,u,temp);
	}
	else
	{
		/* add */
		oid=db_get_objid(arg,OB_USER);
		if (!oid) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		dat=get_attribute(*oid,"passwd");
		if (!dat) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		FREE(dat);
		if (*oid==u->object) {
			swrite(NULL,u,"You don't quite get this do you?\n");
			FREE(oid);
			return;
		}

		insert_into(&lst,arg);
		set_attribute(u->object,"inform",lst);
		sprintf(temp,"You will be informed when %s logs in.\n",arg);
		swrite(NULL,u,temp);
		FREE(oid);
	}

	FREE(lst);
}

void beep(connection *u, char *arg)
{
	char *lst=NULL;
	unsigned long *oid=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a user [fullname] to sound a beep when they login.\n");
		return;
	}

	lst=get_attribute(u->object,"beep");

	if (!lst) {
		lst=MALLOC(2,"beep:lst");
		lst[0]='\0';
	}

	if (in_delim(lst,arg)) {
		/* remove */
		remove_from(&lst,arg);
		set_attribute(u->object,"beep",lst);
		sprintf(temp,"%s removed from beep list.\n",arg);
		swrite(NULL,u,temp);
	}
	else
	{
		/* add */
		oid=db_get_objid(arg,OB_USER);
		if (!oid) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		dat=get_attribute(*oid,"passwd");
		if (!dat) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		FREE(dat);
		if (*oid==u->object) {
			swrite(NULL,u,"You don't quite get this do you?\n");
			FREE(oid);
			return;
		}

		insert_into(&lst,arg);
		set_attribute(u->object,"beep",lst);
		sprintf(temp,"You will be informed with a beep when %s logs in.\n",arg);
		swrite(NULL,u,temp);
		FREE(oid);
	}

	FREE(lst);
}

void key(connection *u, char *arg)
{
	char *lst=NULL;
	unsigned long *oid=NULL;
	connection *target=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a user [fullname] to grant/revoke access to your homeroom.\n");
		return;
	}

	lst=get_attribute(u->object,"keys");

	if (!lst) {
		lst=MALLOC(2,"key:lst");
		lst[0]='\0';
	}

	if (in_delim(lst,arg)) {
		/* remove */
		remove_from(&lst,arg);
		set_attribute(u->object,"keys",lst);
		sprintf(temp,"%s removed from key list.\n",arg);
		swrite(NULL,u,temp);
	}
	else
	{
		/* add */
		oid=db_get_objid(arg,OB_USER);
		if (!oid) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		dat=get_attribute(*oid,"passwd");
		if (!dat) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		FREE(dat);
		if (*oid==u->object) {
			swrite(NULL,u,"You don't quite get this do you?\n");
			FREE(oid);
			return;
		}

		insert_into(&lst,arg);
		set_attribute(u->object,"keys",lst);
		sprintf(temp,"%s has been given a key to your homeroom.\n",arg);
		swrite(NULL,u,temp);
		target=getubyname(NULL,arg,temp);
		if (target) {
			sprintf(temp,"** %s gives you a homeroom key! **\n",u->name);
			swrite(u,target,temp);
		}	
		FREE(oid);
	}

	FREE(lst);
}

void friend(connection *u, char *arg)
{
	char *lst=NULL;
	unsigned long *oid=NULL;
	connection *target=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a user [fullname] to make/break friendship with.\n");
		return;
	}

	lst=get_attribute(u->object,"friends");

	if (!lst) {
		lst=MALLOC(2,"friend:lst");
		lst[0]='\0';
	}

	if (in_delim(lst,arg)) {
		/* remove */
		remove_from(&lst,arg);
		set_attribute(u->object,"friends",lst);
		sprintf(temp,"%s removed from friends list.\n",arg);
		swrite(NULL,u,temp);
	}
	else
	{
		/* add */
		oid=db_get_objid(arg,OB_USER);
		if (!oid) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		dat=get_attribute(*oid,"passwd");
		if (!dat) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		FREE(dat);
		if (*oid==u->object) {
			swrite(NULL,u,"Naaah you dont wanna be friends with that spod!\n");
			FREE(oid);
			return;
		}

		insert_into(&lst,arg);
		set_attribute(u->object,"friends",lst);
		sprintf(temp,"%s has been added to your friends list.\n",arg);
		swrite(NULL,u,temp);
		target=getubyname(NULL,arg,temp);
		if (target) {
			sprintf(temp,"** %s makes you a friend! **\n",u->name);
			swrite(u,target,temp);
		}	
		FREE(oid);
	}

	FREE(lst);
}

void ignore(connection *u, char *arg)
{
	char *lst=NULL;
	unsigned long *oid=NULL;
	connection *target=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify a user [fullname] to ignore/unignore.\n");
		return;
	}

	lst=get_attribute(u->object,"ignores");

	if (!lst) {
		lst=MALLOC(2,"ignore:lst");
		lst[0]='\0';
	}

	if (in_delim(lst,arg)) {
		/* remove */
		remove_from(&lst,arg);
		set_attribute(u->object,"ignores",lst);
		sprintf(temp,"You unignore %s.\n",arg);
		swrite(NULL,u,temp);
	}
	else
	{
		/* add */
		oid=db_get_objid(arg,OB_USER);
		if (!oid) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		dat=get_attribute(*oid,"passwd");
		if (!dat) {
			swrite(NULL,u,"No such user.\n");
			return;
		}
		FREE(dat);
		if (*oid==u->object) {
			swrite(NULL,u,"Just type quit to ignore yourself!\n");
			FREE(oid);
			return;
		}

		insert_into(&lst,arg);
		set_attribute(u->object,"ignores",lst);
		sprintf(temp,"You ignore %s.\n",arg);
		swrite(NULL,u,temp);
		target=getubyname(NULL,arg,temp);
		if (target) {
			sprintf(temp,"** %s ignores you! **\n",u->name);
			swrite(u,target,temp);
		}	
		FREE(oid);
	}

	FREE(lst);
}

/* to avoid nastyness with strtok (spit) we do the following...
**
** copy the first list
** iterate through the remaining adding where the name doesn't exist
**
** iteraten through the big list, checking for entries in all the 
** specific lists, and output.
*/
void list(connection *u, char *arg)
{
	char *biglist=NULL;
	char *uniqlist=NULL;
	char *friends=NULL;
	char *ignores=NULL;
	char *keys=NULL;
	char *beep=NULL;
	char *inform=NULL;
	char *prefer=NULL;
	char temp[TMP_SIZE];
        char *moo=NULL;
        char *ele=NULL;
        char *buf=NULL;

	
	friends=get_attribute(u->object, "friends");
	ignores=get_attribute(u->object, "ignores");
	keys=get_attribute(u->object, "keys");
	beep=get_attribute(u->object, "beep");
	inform=get_attribute(u->object, "inform");
	prefer=get_attribute(u->object, "prefer");

	biglist=MALLOC(strlen(friends?friends:"")+strlen(beep?beep:"")+strlen(prefer?prefer:"")+strlen(inform?inform:"")+strlen(keys?keys:"")+strlen(ignores?ignores:"")+10,"list:biglist");
	uniqlist=MALLOC(strlen(biglist)+10,"list:uniqlist");

	/* insert list categories here */
	biglist[0]='\0';
	uniqlist[0]='\0';
	if (friends) strcat(biglist,friends);
	if (ignores) { 
		if (biglist[0]!='\0') strcat(biglist,",");
		strcat(biglist,ignores);
	}	
	if (keys) { 
		if (biglist[0]!='\0') strcat(biglist,",");
		strcat(biglist,keys);
	}	
	if (beep) { 
		if (biglist[0]!='\0') strcat(biglist,",");
		strcat(biglist,beep);
	}	
	if (inform) { 
		if (biglist[0]!='\0') strcat(biglist,",");
		strcat(biglist,inform);
	}	
	if (prefer) { 
		if (biglist[0]!='\0') strcat(biglist,",");
		strcat(biglist,prefer);
	}	

       	buf=MALLOC(strlen(biglist)+4,"list:buf");
        strcpy(buf,biglist);
        for (ele=strtok_r(buf,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
        {
		if (!in_delim(uniqlist,ele)) {
			insert_into(&uniqlist,ele);
		}	
        }
        FREE(buf);

	if (uniqlist[0]=='\0') {
		swrite(NULL,u,"Your list is empty.\n");
	}
	else
	{
		format_bar(u, NULL,temp);
		swrite(NULL,u,temp);
	       	buf=MALLOC(strlen(uniqlist)+4,"list:buf");
       		strcpy(buf,uniqlist);
       		for (ele=strtok_r(buf,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
       		{
			sprintf(temp,"%-20s :",ele);
			if (friends) {
				strcat(temp,in_delim(friends,ele)?" Friend":"");
			}
			if (ignores) {
				strcat(temp,in_delim(ignores,ele)?" Ignore":"");
			}	
			if (keys) {
				strcat(temp,in_delim(keys,ele)?" Key":"");
			}	
			if (beep) {
				strcat(temp,in_delim(beep,ele)?" Beep":"");
			}	
			if (inform) {
				strcat(temp,in_delim(inform,ele)?" Inform":"");
			}	
			if (prefer) {
				strcat(temp,in_delim(prefer,ele)?" Prefer":"");
			}	
			strcat(temp,"\n");
			
			swrite(NULL,u,temp);
       		}
       		FREE(buf);
		format_bar(u, NULL,temp);
		swrite(NULL,u,temp);
	}	

	if (friends) FREE(friends);
	if (ignores) FREE(ignores);
	if (keys) FREE(keys);
	if (beep) FREE(beep);
	if (inform) FREE(inform);
	if (prefer) FREE(prefer);
	FREE(biglist);
	FREE(uniqlist);
}

void event(connection *u,short id, char *passed, char **ret)
{
	connection *sweep=NULL;
	char *inform=NULL;
	char temp[TMP_SIZE];
	char *dat=NULL;

	if (id==E_UNIDLE) {
		if (time(0)-u->last_command<1800) return;
		dat=get_attribute(u->object,"muffle_idle");
		if (dat) {
			if (*dat=='Y') {
				FREE(dat);
				return;
			}
			FREE(dat);
		}
		sprintf(temp,"[^YINFORM^N] %s has woken after being idle for %s.\n",u->name,timeval(time(0)-u->last_command));
		for (sweep=users;sweep;sweep=sweep->next) {
			inform=get_attribute(sweep->object,"inform");
			if (inform) {
				if (in_delim(inform, u->name)) {
					swrite(NULL,sweep,temp);
				}
				FREE(inform);
			}
		}	

		sprintf(temp,"[^YINFORM^N] You were idle for %s.\n",timeval(time(0)-u->last_command));
		swrite(NULL,u,temp);
	}
}

int init()
{
	add_command("friend",&friend,NULL,NULL);
	add_command("ignore",&ignore,NULL,NULL);
	add_command("key",&key,NULL,NULL);
	add_command("beep",&beep,NULL,NULL);
	add_command("inform",&inform,NULL,NULL);
	add_command("prefer",&prefer,NULL,NULL);
	add_command("list",&list,NULL,NULL);
	return 1;
}	

int uninit()
{
	remove_command("friend",NULL);
	remove_command("ignore",NULL);
	remove_command("key",NULL);
	remove_command("beep",NULL);
	remove_command("inform",NULL);
	remove_command("prefer",NULL);
	remove_command("list",NULL);
	return 1;
}	

char *version()
{
	return "Lists (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
