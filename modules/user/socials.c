/* $Id: socials.c,v 2.1 2005/10/25 11:42:38 ksb  Exp $ */
/* $Revision: 2.1 $ */

#include "flump.h"

/* okay for the socials we need...

is there a target? yes/no
can it be self-targeted? yes/no
does it require a target? yes/no

if target only...
	how will it look for the user
	how will it look for the room
	how will it look for the target

+if self-targeted...
	how will it look to the user
	how will it look to the room
	
+if no-target...
	how will it look for the user
	how will it look for the room

*/	

extern pthread_mutex_t db_mutex;
	
void yay(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You go ^GY^BA^GY^N at %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s goes ^GY^BA^GY^N at %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You go ^GY^BA^GY^N.\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s goes ^GY^BA^GY^N.\n",u->name);
		sroom(u,temp);
	}
}

void dance(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You ^Gdance^N with %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Gdances^N with %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You ^Gdance^N with yourself... feeling lonely?\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Gdances^N around the room... poor lonely fool!\n",u->name);
		sroom(u,temp);
	}
}

void tachyon(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You ^Rfire^N a ^BTachyon burst^N at %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Rfires^N a ^BTachyon burst^N at %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You ^Rfire^N ^BTachyons^N at everyone.\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Rfloods^N the room with ^BTachyons^N.\n",u->name);
		sroom(u,temp);
	}
}

void hug(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Who do you want to hug?\n");
		return;
	}

	t=getubyname(u,arg,temp);
	if (!t) {
		swrite(NULL,u,temp);
		return;
	}
	/* got name */
	if (t==u) isme=1;

	sprintf(temp,"You ^Bhug^N %s.\n",isme?"yourself":t->name);
	swrite(NULL,u,temp);
	sprintf(temp,"%s ^Bhugs^N %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
	sroom(u,temp);
}

void lick(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Who do you want to lick?\n");
		return;
	}

	t=getubyname(u,arg,temp);
	if (!t) {
		swrite(NULL,u,temp);
		return;
	}
	/* got name */
	if (t==u) isme=1;

	sprintf(temp,"You ^Glick^N %s, ^YSl^Buuuuuuu^Mrp^C!!!^N.\n",isme?"yourself":t->name);
	swrite(NULL,u,temp);
	sprintf(temp,"%s ^Glicks^N %s, ^YSl^Buuuuuuu^Mrp^C!!!^N\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
	sroom(u,temp);
}

void smooch(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Who do you want to smooch?\n");
		return;
	}

	t=getubyname(u,arg,temp);
	if (!t) {
		swrite(NULL,u,temp);
		return;
	}
	/* got name */
	if (t==u) isme=1;

	sprintf(temp,"You ^YS^ym^BO^bo^RO^ro^MO^mo^GO^gc^HH^N %s.\n",isme?"yourself":t->name);
	swrite(NULL,u,temp);
	sprintf(temp,"%s ^YS^ym^BO^bo^RO^ro^MO^mo^GO^gc^HH^Be^bS^N %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
	sroom(u,temp);
}

void snog(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Who do you want to snog?\n");
		return;
	}

	t=getubyname(u,arg,temp);
	if (!t) {
		swrite(NULL,u,temp);
		return;
	}
	/* got name */
	if (t==u) isme=1;

	sprintf(temp,"You ^Ys^Gn^Bo^Pg %s.\n",isme?"yourself":t->name);
	swrite(NULL,u,temp);
	sprintf(temp,"%s ^Ys^Gn^Bo^Pg^Rs %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
	sroom(u,temp);
}

void whip(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Who do you want to whip?\n");
		return;
	}

	t=getubyname(u,arg,temp);
	if (!t) {
		swrite(NULL,u,temp);
		return;
	}
	/* got name */
	if (t==u) isme=1;

	sprintf(temp,"With a ^YKerplish^N you ^Bwhip^N %s.\n",isme?"yourself":t->name);
	swrite(NULL,u,temp);
	sprintf(temp,"With a ^YKerplish^N %s ^Bwhips^N %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
	sroom(u,temp);
}

void mosh(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You ^Gmosh ^Baround ^Mthe ^Yroom with %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Gmoshes ^Baround ^Mthe ^Yroom with %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You ^Gmosh ^Baround ^Mthe ^Yroom^N.\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Gmoshes ^Baround ^Mthe ^Yroom^N.\n",u->name);
		sroom(u,temp);
	}
}

void nod(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You ^Bnod^N at %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Bnods^N at %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You ^Bnod^N.\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Bnods^N.\n",u->name);
		sroom(u,temp);
	}
}

void lol(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You ^Blaugh ^Gout ^Ploud^N at %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Blaughs ^Gout ^Ploud^N at %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You ^Blaugh ^Gout ^Ploud^N.\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Blaughs ^Gout ^Ploud^N.\n",u->name);
		sroom(u,temp);
	}
}

void grin(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You ^Ggrin^N at %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Ggrins^N at %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You ^Ggrin^N.\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Ggrins^N.\n",u->name);
		sroom(u,temp);
	}
}

void wave(connection *u, char *arg)
{
	connection *t=NULL;
	int isme=0;
	char temp[TMP_SIZE];

	if (arg) {
		t=getubyname(u,arg,temp);
		if (!t) {
			swrite(NULL,u,temp);
			return;
		}
		/* got name */
		if (t==u) isme=1;
	}

	if (t) {
		sprintf(temp,"You ^Bwave^N at %s.\n",isme?"yourself":t->name);
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Bwaves^N at %s.\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name);
		sroom(u,temp);
	}
	else
	{
		sprintf(temp,"You ^Bwave^N.\n");
		swrite(NULL,u,temp);
		sprintf(temp,"%s ^Bwaves^N.\n",u->name);
		sroom(u,temp);
	}
}


/* Slap social - requires data and table in slap.sql schema file */
void slap(connection *u, char *arg)
{
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	connection *t=NULL;
	char temp[TMP_SIZE];
	int isme=0;
	int ernum;

	if(!arg)
	{
		swrite(NULL,u,"Who do you want to slap?\n");
		return;
	}
	t=getubyname(u,arg,temp);
	if(!t)
	{
		swrite(NULL,u,temp);
		return;
	}

	if(t==u) isme=1;

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,"SELECT slap from slap order by rand() limit 0,1");
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	result = mysql_store_result(global.database);
	row = mysql_fetch_row(result);
	sprintf(temp,"You ^Cslap^N %s ^Yround the ^Rface ^Ywith a ^C%s^Y.^N\n",isme?"yourself":t->name,row[0]);
	swrite(NULL,u,temp);
	sprintf(temp,"%s ^Cslaps^N %s ^Yround the ^Rface ^Ywith a ^C%s^Y.^N\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name,row[0]);
	sroom(u,temp);
	MYSQL_FREE(result);
	pthread_mutex_unlock(&db_mutex);
}

/* Stab social - requires data and table in stab.sql schema file */
void stab(connection *u, char *arg)
{
        MYSQL_RES *result=NULL;
        MYSQL_ROW row;
	connection *t=NULL;
	char temp[TMP_SIZE];
	int isme=0;
	int ernum;
	char *item;
	char *location;

	if(!arg)
	{
		swrite(NULL,u,"Who do you want to stab?\n");
		return;
	}
	t=getubyname(u,arg,temp);
	if(!t)
	{
		swrite(NULL,u,temp);
		return;
	}

	if(t==u) isme=1;

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,"SELECT item from stab_items order by rand() limit 0,1");
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	result = mysql_store_result(global.database);
	row = mysql_fetch_row(result);
	item=strdup(row[0]);
	MYSQL_FREE(result);

	pthread_mutex_lock(&db_mutex);
	ernum = mysql_query(global.database,"SELECT location from stab_locations order by rand() limit 0,1");
	if(ernum!=0)
        {
                debug("Database Select Failed:");
                debug((char *)mysql_error(global.database));
                debug(temp);
                pthread_mutex_unlock(&db_mutex);
                return;
        }
	result = mysql_store_result(global.database);
	row = mysql_fetch_row(result);
	location=strdup(row[0]);
	MYSQL_FREE(result);


	sprintf(temp,"You ^Rstab^N %s ^Yin the ^R%s ^Ywith a ^C%s^Y.^N\n",isme?"yourself":t->name,location,item);
	swrite(NULL,u,temp);
	sprintf(temp,"%s ^Rstabs^N %s ^Yin the ^R%s ^Ywith a ^C%s^Y.^N\n",u->name,isme?getgen(u,"himself","herself","itself"):t->name,location,item);
	sroom(u,temp);
	free(item);
	free(location);
	pthread_mutex_unlock(&db_mutex);
}

	

	


	

int init()
{
	add_command("yay",&yay,NULL,NULL);
	add_command("dance",&dance,NULL,NULL);
	add_command("hug",&hug,NULL,NULL);
	add_command("lol",&lol,NULL,NULL);
	add_command("nod",&nod,NULL,NULL);
	add_command("grin",&grin,NULL,NULL);
	add_command("wave",&wave,NULL,NULL);
	add_command("mosh",&mosh,NULL,NULL);
	add_command("smooch",&smooch,NULL,NULL);
	add_command("slap",&slap,NULL,NULL);
	add_command("stab",&stab,NULL,NULL);
	add_command("tachyon",&tachyon,NULL,NULL);
	add_command("whip",&whip,NULL,NULL);
	add_command("lick",&lick,NULL,NULL);
	add_command("snog",&snog,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("yay",NULL);
	remove_command("dance",NULL);
	remove_command("hug",NULL);
	remove_command("lol",NULL);
	remove_command("nod",NULL);
	remove_command("grin",NULL);
	remove_command("wave",NULL);
	remove_command("mosh",NULL);
	remove_command("smooch",NULL);
	remove_command("slap",NULL);
	remove_command("stab",NULL);
	remove_command("tachyon",NULL);
	remove_command("whip",NULL);
	remove_command("lick",NULL);
	remove_command("snog",NULL);
	return 1;
}

char *version()
{
	return "Socials (c) 2004 Karl Bastiman $Revision: 2.1 $";
}
