/* $Id: privs.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

/* Right....
**
** to keep this all as easy as possible a priv is an object, it's done this
** way so when a priv is removed it will immediately effect any commands and
** users associated with it.
**
** users/commands/etc are attributes of each priv object and contain a comma 
** seperated list.
**
** this leaves the problem of how to identify the priv objects (as one needs
** to know name and object-type, so this is where it gets a little dodgy, the
** create_priv function will create or add to a 'priv' object with details of
** all current priv objects in the db, this will naturally need to be altered
** on deletion too.
*/

/* assign a priv to a user 
*/
void grant_priv(connection *u, char *arg)
{
	unsigned long *objid=NULL;
	char *usr=NULL;
	char *allusr=NULL;
	char temp[TMP_SIZE];
	connection *tu=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify priv name and a user.\n");
		return;
	}	
	usr=strchr(arg,' ');

	if (!usr) {
		swrite(NULL,u,"A user name is needed.\n");
		return;
	}

	*usr='\0';
	usr++;

	objid=db_get_objid(usr,OB_USER);

	if (!objid) {
		swrite(NULL,u,"No such user.\n");
		return;
	}

	FREE(objid);

	objid=db_get_objid(arg,OB_PRIV);

	if (!objid) {
		swrite(NULL,u,"No such priv.\n");
		return;
	}

	allusr=get_attribute(*objid,"users");

	if (!allusr) {
		allusr=MALLOC(2*sizeof(char),"grant_priv:allusr");
		allusr[0]='\0';
	}

	insert_into(&allusr,usr);
	set_attribute(*objid,"users",allusr);

	sprintf(temp,"%s added to %s priv.\n",usr,arg);
	swrite(NULL,u,temp);

	tu=getubyname(NULL,usr,temp);
	if (tu) {
		sprintf(temp,"%s has granted you %s priv.\n",u->name,arg);
		swrite(NULL,tu,temp);
	}	
	FREE(objid);
	FREE(allusr);
}

void revoke_priv(connection *u, char *arg)
{
	unsigned long *objid=NULL;
	char *usr=NULL;
	char *allusr=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Specify priv name and a user.\n");
		return;
	}	

	usr=strchr(arg,' ');

	if (!usr) {
		swrite(NULL,u,"A user name is needed.\n");
		return;
	}

	*usr='\0';
	usr++;

/*
	objid=db_get_objid(usr,OB_USER);

	if (!objid) {
		swrite(NULL,u,"No such user.\n");
		return;
	}

	FREE(objid);

*/
	objid=db_get_objid(arg,OB_PRIV);

	if (!objid) {
		swrite(NULL,u,"No such priv.\n");
		return;
	}

	allusr=get_attribute(*objid,"users");

	if (!allusr) {
		allusr=MALLOC(2*sizeof(char),"revoke_priv:allusr");
		allusr[0]='\0';
	}

	if (!in_delim(allusr,usr)) {
		swrite(NULL,u,"User does not have that priv (or does not exist).\n");
	}
	else
	{
		remove_from(&allusr,usr);
		set_attribute(*objid,"users",allusr);

		sprintf(temp,"%s removed from %s priv.\n",usr,arg);
		swrite(NULL,u,temp);
	}	

	FREE(objid);
	FREE(allusr);
}

/* create a priv
*/
void create_priv(connection *u, char *arg)
{
	char *desc=NULL;
	char *att=NULL;
	unsigned long *objid=NULL;

	if (!arg) {
		swrite(NULL,u,"Specify priv name and description.\n");
		return;
	}	
	desc=strchr(arg,' ');

	if (!desc) {
		swrite(NULL,u,"A description is needed.\n");
		return;
	}

	*desc='\0';
	desc++;

	if (strlen(arg)<2||strlen(arg)>15) {
		swrite(NULL,u,"The priv name must be between 2 and 15 characters.\n");
		return;
	}

	objid=db_set_newobj(arg, OB_PRIV);
	set_attribute(*objid,"desc",desc);

	objid=db_get_objid("privs",OB_PRIV);
	if (!objid) {
		objid=db_set_newobj("privs",OB_PRIV);
	}	
	att=get_attribute(*objid,"index");
	if (!att) {
		/* create a small malloc */
		att=MALLOC(10,"create_priv:att");
		att[0]='\0';
	}
	else
	{
		if (in_delim(att,arg)) {
			swrite(NULL,u,"Priv already exists.\n");
			FREE(att);
			FREE(objid);
			return;
		}	
	}	
	insert_into(&att,arg);
	set_attribute(*objid,"index",att);
	FREE(att);
	FREE(objid);
}

void remove_priv(connection *u, char *arg)
{
	unsigned long *objid=NULL;
	unsigned long *privid=NULL;
	char *att=NULL;

	if (!arg) {
		swrite(NULL,u,"Need to specify a priv to remove.\n");
		return;
	}

	objid=db_get_objid(arg,OB_PRIV);

	if (!objid) {
		swrite(NULL,u,"No such priv in DB.\n");
		return;
	}

	privid=db_get_objid("privs",OB_PRIV);

	/* this should exist if it doesn't, we have issues */

	if (!privid) {
		swrite(NULL,u,"Error, see debug logs!\n");
		debug("FATAL:priv exists but priv object doesn't.");
		FREE(objid);
		return;
	}
	
	att=get_attribute(*privid,"index");

	if (!att) {
		swrite(NULL,u,"Error, see debug logs!\n");
		debug("FATAL:priv exists, priv object exists, NO index.");
		FREE(objid);
		FREE(privid);
		return;
	}	

	remove_from(&att,arg);
	set_attribute(*privid,"index",att);

	db_delete_obj(*objid);

	FREE(objid);
	FREE(privid);
	FREE(att);
}

int priv_match(connection *u, command *cmd)
{
	unsigned long *objid=NULL;
	char *priv=NULL;
	char *allusr=NULL;
	int ret=0;

	objid=db_get_objid(cmd->name,OB_COMMAND);

	if (!objid) {
		return 1;
	}

	priv=db_get_attribute(*objid, "rank");

	FREE(objid);

	if (!priv) {
		return 1;
	}

	objid=db_get_objid(priv,OB_PRIV);

	if (!objid) {
		/* odd the priv assigned to *command* doesnt exist */
		debug("priv_match: no such priv, sort it out!");
		return 0;
	}

	allusr=get_attribute(*objid,"users");

	if (!allusr) {
		/* priv exists but no users assigned, this is valid */
		return 0;
	}
	
	if (in_delim(allusr, u->name)) {
		ret=1;
	}
	else
	{
		ret=0;
	}

	FREE(objid);
	FREE(priv);
	FREE(allusr);
	return ret;
}

/* make it so a command can only be used by priv level
** users.
*/
void priv_command(connection *u, char *arg)
{
	char *priv=NULL;
	unsigned long *objid=NULL;
	unsigned long *privobj=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Specify a command.\n");
		return;
	}

	priv=strchr(arg,' ');

	if (!priv) {
		swrite(NULL,u,"You need to specify a priv to assign.\n");
		return;
	}

	*priv='\0';
	priv++;
	privobj=db_get_objid(priv,OB_PRIV);

	if (!privobj) {
		swrite(NULL,u,"No such priv.\n");
		return;
	}	

	if (!is_command(arg)) {
		swrite(NULL,u,"No such command.\n");
		return;
	}

	objid=db_get_objid(arg,OB_COMMAND);

	if (!objid) {
		objid=db_set_newobj(arg,OB_COMMAND);
	}

	set_attribute(*objid,"rank",priv);
	sprintf(temp,"Command %s set to rank %s.\n",arg,priv);
	swrite(NULL,u,temp);

	FREE(objid);
	FREE(privobj);
}

void unpriv_command(connection *u, char *arg)
{
	unsigned long *objid=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Specify a command.\n");
		return;
	}

	if (!is_command(arg)) {
		swrite(NULL,u,"No such command.\n");
		return;
	}

	objid=db_get_objid(arg,OB_COMMAND);

	if (!objid) {
		swrite(NULL,u,"That command has no priv assigned.\n");
		return;
	}

	db_delete_obj(*objid);
	sprintf(temp,"all privs removed from command %s.\n",arg);
	swrite(NULL,u,temp);
	FREE(objid);
}

/* a simple staff command
** this can ofcourse be priv'd out of reach if you want to
*/
void show_staff(connection *u, char *arg)
{
	char *index=NULL;
	unsigned long *objid=NULL;
	char *moo=NULL;
	char *ele=NULL;
	char temp[TMP_SIZE];
	char *usr=NULL;

	objid=db_get_objid("privs",OB_PRIV);

	if (!objid) {
		swrite(NULL,u,"There are no staff yet.\n");
		return;
	}

	index=get_attribute(*objid,"index");

	if (!index) {
		swrite(NULL,u,"There are no staff yet.\n");
		FREE(objid);
		return;
	}

	if (index[0]=='\0') {
		swrite(NULL,u,"There are no staff yet.\n");
		FREE(objid);
		FREE(index);
		return;
	}
	
	FREE(objid);

	for (ele=strtok_r(index,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		objid=db_get_objid(ele,OB_PRIV);
		if (!objid) {
			debug("Privs need a tidy, no object for index relation.");
			continue;
		}
		usr=get_attribute(*objid,"users");
		if (!usr) {
			usr=MALLOC(2*sizeof(char),"show_staff:usr");
			usr[0]='\0';
		}
		sprintf(temp,"%s : %s\n",ele,usr[0]=='\0'?"empty":usr);
		swrite(NULL,u,temp);
		FREE(objid);
		FREE(usr);
	}

	FREE(index);
}

int init()
{
	add_command("addpriv",&create_priv,NULL,NULL);
	add_command("delpriv",&remove_priv,NULL,NULL);
	add_command("privcmd",&priv_command,NULL,NULL);
	add_command("unprivcmd",&unpriv_command,NULL,NULL);
	add_command("grant",&grant_priv,NULL,NULL);
	add_command("revoke",&revoke_priv,NULL,NULL);
	add_command("staff",&show_staff,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("addpriv",NULL);
	remove_command("delpriv",NULL);
	remove_command("privcmd",NULL);
	remove_command("unprivcmd",NULL);
	remove_command("grant",NULL);
	remove_command("revoke",NULL);
	remove_command("staff",NULL);
	return 1;
}


char *version()
{
	return "Privs system (c) 2004 Karl Bastiman $Revision: 2.0 $";
}

