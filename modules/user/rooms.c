	/* $Id: rooms.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

mode roommode;
int tickcount=0;

void room_create(connection *u, char *arg)
{
	unsigned long *obj=NULL;
	int ret=0;

	/* room names should be between 3 and 15 chars
	** no non alphanumerics
	*/
	if (!arg) {
		swrite(NULL,u,"Specify a room.\n");
		return;
	}

	if (strlen(arg)<3) {
		swrite(NULL,u,"Room name too short.\n");
		return;
	}

	if (strlen(arg)>15) {
		swrite(NULL,u,"Room name too long.\n");
		return;
	}

	if (strchr(arg,' ')) {
		swrite(NULL,u,"No spaces in room names.\n");
		return;
	}

	if (strncasecmp(arg,"home",4)==0) {
		swrite(NULL,u,"Homerooms are automatically created.\n");
		return;
	}

	ret=create_room(arg);
	if (ret!=0) {
		swrite(NULL,u,"Room creation failed (may be a duplicate.\n");
		return;
	}

	obj=db_get_objid(arg, OB_ROOM);

	if (!obj) {
		swrite(NULL,u,"Unknown error!\n");
		return;
	}

	set_attribute(*obj,"owner",u->name);

	swrite(NULL,u,"Room created.\n");
	FREE(obj);
}

void tick()
{
	connection *sweep=NULL;
	time_t truespod=0;
	char *dat=NULL;
	unsigned long *obj=NULL;
	int ret=0;
	char temp[TMP_SIZE];

	tickcount++;
	if (tickcount<60) return;
	tickcount=0;

	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		dat=get_attribute(sweep->object,"homeroom");
		if (dat) {
			FREE(dat);
			continue;
		}

		dat=get_attribute(sweep->object,"truespod");
		truespod=0;
		if (dat) {
			truespod=atol(dat);
			truespod+=sweep->ctruespod;
			FREE(dat);
		}
		if (truespod>172800) {
			swrite(NULL,sweep,"^B*>^N With 2 days truespod time you have earned a homeroom, have fun!\n");
			sprintf(temp,"home%s",sweep->name);
			ret=create_room(temp);
			if (ret!=0) {
				swrite(NULL,sweep,"Room creation failed (may be a duplicate).\n");
				continue;
			}

			obj=db_get_objid(temp, OB_ROOM);
			if (!obj) {
				swrite(NULL,sweep,"Unknown error!\n");
				continue;	
			}
			set_attribute(*obj,"owner",sweep->name);
			set_attribute(sweep->object,"homeroom",temp);
			FREE(obj);
		}
	}	
}

/* right we should just be able to remove the room object, the users in the 
** room will be dumped back to main, if main is removed then it should be 
** recreated automagically
*/
void room_remove(connection *u, char *arg)
{
	unsigned long *obj=NULL;

	/* right, better check arg anyway */

	if (!arg) {
		swrite(NULL,u,"Specify a room.\n");
		return;
	}

	if (strlen(arg)<3) {
		swrite(NULL,u,"Room name too short.\n");
		return;
	}

	if (strlen(arg)>15) {
		swrite(NULL,u,"Room name too long.\n");
		return;
	}

	if (strchr(arg,' ')) {
		swrite(NULL,u,"No spaces in room names.\n");
		return;
	}

	obj=db_get_objid(arg, OB_ROOM);

	if (!obj) {
		swrite(NULL,u,"No such room.\n");
		return;
	}

	db_delete_obj(*obj);

	FREE(obj);

	swrite(NULL,u,"Room deleted.\n");
}

/* this func should check to see if the room is locked or if the user is invited
** but right now it just moves the user
*/
void room_go(connection *u, char *arg)
{
	/* right, better check arg anyway */
	unsigned long *obj=NULL;
	unsigned long *obj2=NULL;
	char *uroom=NULL;
	char temp[TMP_SIZE];

	if (!arg) {
		swrite(NULL,u,"Specify a room.\n");
		return;
	}

	if (strlen(arg)<3) {
		swrite(NULL,u,"Room name too short.\n");
		return;
	}

	if (strlen(arg)>15) {
		swrite(NULL,u,"Room name too long.\n");
		return;
	}

	if (strchr(arg,' ')) {
		swrite(NULL,u,"No spaces in room names.\n");
		return;
	}

	obj=db_get_objid(arg, OB_ROOM);

	if (!obj) {
		swrite(NULL,u,"No such room.\n");
		return;
	}

	uroom=get_attribute(u->object,"room");
	if (uroom) obj2=db_get_objid(uroom, OB_ROOM);

	if (uroom&&obj2) {
		/* if the users current room is still there then post a 
		** message saying they have left
		*/
		sprintf(temp,"- %s leaves for another room.\n",u->name);
		sroom(u, temp);
		FREE(uroom);
		FREE(obj2);
	}	

	set_attribute(u->object,"room",arg);

	look(u,NULL);
	sprintf(temp,"= %s has arrived.\n",u->name);
	sroom(u, temp);

	FREE(obj);
}

void visit(connection *u, char *arg)
{
	char *uroom=NULL;
	char *croom=NULL;
	char *keys=NULL;
	char temp[TMP_SIZE];
	connection *t=NULL;

	if (!arg) {
		swrite(NULL,u,"Visit who?\n");
		return;
	}

	t=getubyname(u,arg,temp);

	if (!t) {
		swrite(NULL,u,temp);
		return;
	}

	if (t==u) {
		swrite(NULL,u,"Use the home command instead.\n");
		return;
	}

	croom=get_attribute(u->object,"room");
	if (!croom) {
		swrite(NULL,u,"Internal fuckup, whoops.\n");
		return;
	}	
	uroom=get_attribute(t->object,"homeroom");
	if (!uroom) {
		swrite(NULL,u,"They do not have a homeroom yet.\n");
		FREE(croom);
		return;
	}

	if (strcasecmp(croom,uroom)==0) {
		swrite(NULL,u,"You are already visiting them.\n");
		FREE(croom);
		FREE(uroom);
		return;
	}	

	keys=get_attribute(t->object,"keys");
	if (keys) {
		if (!in_delim(keys,u->name)) {
			swrite(NULL,u,"You need a key to enter that room.\n");
			FREE(keys);
			return;
		}
		FREE(keys);
	}	
	else
	{
		swrite(NULL,u,"You need a key to enter that room.\n");
		return;
	}	
	sprintf(temp,"- %s leaves for %s's homeroom.\n",u->name,t->name);
	sroom(u, temp);
	set_attribute(u->object,"room",uroom);
	look(u,NULL);
	sprintf(temp,"= %s has arrived.\n",u->name);
	sroom(u, temp);
	FREE(croom);
	FREE(uroom);
}

void home(connection *u, char *arg)
{
	char *uroom=NULL;
	char *croom=NULL;
	char temp[TMP_SIZE];

	croom=get_attribute(u->object,"room");
	if (!croom) {
		swrite(NULL,u,"Internal fuckup, whoops.\n");
		return;
	}	
	uroom=get_attribute(u->object,"homeroom");
	if (!uroom) {
		swrite(NULL,u,"You do not have a homeroom yet.\n");
		FREE(croom);
		return;
	}

	if (strcasecmp(croom,uroom)==0) {
		swrite(NULL,u,"You are already home.\n");
		FREE(croom);
		FREE(uroom);
		return;
	}	

	sprintf(temp,"- %s leaves for %s homeroom.\n",u->name,getgen(u,"his","her","its"));
	sroom(u, temp);
	set_attribute(u->object,"room",uroom);
	look(u,NULL);
	sprintf(temp,"= %s has arrived.\n",u->name);
	sroom(u, temp);
	FREE(croom);
	FREE(uroom);
}

void gomain(connection *u, char *arg)
{
	char temp[TMP_SIZE];
	char *croom=NULL;
	
	croom=get_attribute(u->object,"room");
	if (!croom) {
		swrite(NULL,u,"Internal fuckup, whoops.\n");
		return;
	}	
	if (strcasecmp(croom,"main")==0) {
		swrite(NULL,u,"You are already in the main room.\n");
		FREE(croom);
		return;
	}
	FREE(croom);

	sprintf(temp,"- %s leaves for the main room.\n",u->name);
	sroom(u, temp);
	set_attribute(u->object,"room","main");
	look(u,NULL);
	sprintf(temp,"= %s has arrived.\n",u->name);
	sroom(u, temp);
}

void room_list(connection *u, char *arg)
{
}

void roomstart(connection *u, char *arg)
{
	swrite(NULL,u,"Entering ROOM mode.\n");
	u->usermode=&roommode;
}

void roomend(connection *u, char *arg)
{
	swrite(NULL,u,"Leaving ROOM mode.\n");
	u->usermode=NULL;
}

int init()
{
	roommode.prompt=MALLOC(128,"init:roommode.prompt");
	strcpy(roommode.prompt,"room>");
	add_command("go",&room_go,NULL,NULL);
	add_command("room",&roomstart,NULL,NULL);
	add_command("home",&home,NULL,NULL);
	add_command("main",&gomain,NULL,NULL);
	add_command("leave",NULL,"main",NULL);
	add_command("visit",&visit,NULL,NULL);
	add_command("end",&roomend,NULL,&roommode);
	add_command("commands",&listcommands,NULL,&roommode);
	add_command("list",&room_list,NULL,&roommode);
	add_command("delete",&room_remove,NULL,&roommode);
	add_command("make",&room_create,NULL,&roommode);
	return 1;
}

int uninit()
{
	/* users will now be trapped in their rooms ;) */
	FREE(roommode.prompt);
	remove_command("end",&roommode);
	remove_command("room",NULL);
	remove_command("main",NULL);
	remove_command("home",NULL);
	remove_command("visit",NULL);
	remove_command("commands",&roommode);
	remove_command("go",NULL);
	remove_command("list",&roommode);
	remove_command("delete",&roommode);
	remove_command("make",&roommode);
	return 1;
}

char *version()
{
	return "Room code (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
