/* $Id: admin.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

void ushutdown(connection *u,char *arg)
{
        global.running=0;
}

void nuke(connection *u, char *arg)
{	
	connection *t=NULL;
	char temp[TMP_SIZE];
	long obj=0;

	if (!arg) {
		swrite(NULL,u,"Specify a name to nuke.\n");
		return;
	}

	t=getubyname(u,arg,temp);

	if (!t) {
		swrite(NULL,u,temp);
		return;
	}

	obj=t->object;
	swrite(NULL,t,"YOU HAVE BEEN ^RNUKED^N. Have a nice day!\n\n");
	sprintf(temp,"^R-->^N %s has ^RNUKED^N %s, shame eh? ^R<--^N\n",u->name,t->name);
	swall(t,temp);
	t->destroy=1;
}

void boot(connection *u, char *arg)
{	
	connection *t=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Specify a name to boot.\n");
		return;
	}

	t=getubyname(u,arg,temp);

	if (!t) {
		swrite(NULL,u,temp);
		return;
	}

	sprintf(temp,"^G-->^N %s has been booted by %s for being a pratt. ^G<--^N\n",t->name,u->name);
	swall(t,temp);
	sprintf(temp,"  You have been ^Rbooted^N by %s, come back when you can behave!\n",u->name);
	swrite(NULL,t,temp);
	t->disconnect=1;
}


void banname(connection *u, char *arg)
{
}

int init()
{
	add_command("shutdown",&ushutdown,NULL,NULL);
	set_command_flag("shutdown",C_NOSHORT);
	add_command("nuke",&nuke,NULL,NULL);
	set_command_flag("nuke",C_NOSHORT);
	add_command("boot",&boot,NULL,NULL);
	set_command_flag("boot",C_NOSHORT);
	return 1;
}

int uninit()
{
	remove_command("shutdown",NULL);
	remove_command("nuke",NULL);
	remove_command("boot",NULL);
	return 1;
}

char *version()
{
	return "Admin module (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
