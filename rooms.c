/* $Id: rooms.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

/* this isnt done as a module because there will be too much interaction
** from other modules. -ksb
*/

/* we need to cope with the talker having no rooms, as some tit might 
** remove them all so it's not enough to just populate the db with an
** initial 'main' room.
**
** In order to avoid massive fuck up I'm going to check each user on
** any given room communication to see if their room is still there, if
** it isnt then move them to the main room, if that doesnt exist then
** recreate the main room, this should basically have the effect that
** on first startup, the first say/emote/etc will create a main room.
**
** We are not using room descriptions basically because this isnt a MUD.
*/

/* creates a room of name 'roomname'
** if 'roomname' is already created then it will return -1
** other wise 0 is returned for success
*/
#include <flump.h>

/* arg is ignored */
void look(connection *u, char *arg)
{
	char *uroom=NULL;
	char *swroom=NULL;
	connection *sweep=NULL;
	int num=0;
	char temp[TMP_SIZE];
	char *title=NULL;
	unsigned long *objid=NULL;

	debug("look");

	uroom=get_user_room(u);

	if (strncmp(uroom,"home",4)==0) {
		sprintf(temp,"^B[^GYou are in %s^G's homeroom^B]^N\n\n",uroom+4);
		swrite(NULL,u,temp);
	}
	else
	{
		sprintf(temp,"^B[^GYou are in the %s room^B]^N\n\n",uroom);
		swrite(NULL,u,temp);
	}	

	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep==u) continue;
		if (sweep->quiet) continue;
		swroom=get_user_room(sweep);
		if (!swroom) continue;
		if (strcmp(swroom,uroom)==0) num++;
		FREE(swroom);
	}

	switch (num) {
		case 0: sprintf(temp,"There is nobody else here.\n"); break;
		case 1: sprintf(temp,"There is one other person here.\n"); break;
		default: sprintf(temp,"There are %d other people here.\n",num); break;
	}

	swrite(NULL,u,temp);
	temp[0]='\0';
	if (num) {
		if (num<10) {
			for (sweep=users;sweep;sweep=sweep->next) {
				if (sweep==u) continue;
				if (sweep->quiet) continue;
				swroom=get_user_room(sweep);
				if (!swroom) continue;
				if (strcmp(swroom,uroom)==0) {
					objid=db_get_objid(sweep->name,OB_USER);
					if (objid) {
						/* if we dont get here it's fucked anyway */
						title=get_attribute(*objid,"title");
						if (!title) {
							title=MALLOC(2*sizeof(char),"look:title");
							title[0]='\0';
						}	
						FREE(objid);
					}	
					sprintf(temp,"%s%s%s\n",sweep->name,*title==','||*title=='\''?"":" ",title);
					swrite(NULL,u,temp);
					FREE(title);
				}	
				FREE(swroom);
			}
		}	
		else
		{
			for (sweep=users;sweep;sweep=sweep->next) {
				if (sweep==u) continue;
				if (sweep->quiet) continue;
				swroom=get_user_room(sweep);
				if (!swroom) continue;
				if (strcmp(swroom,uroom)==0) {
					if (strlen(temp)+strlen(sweep->name)>75) {
						strcat(temp,"\n");
						swrite(NULL,u,temp);
						temp[0]='\0';
					}	
					if (temp[0]!='\0') {
						strcat(temp,", ");
					}	
					strcat(temp,sweep->name);
				}
			}	
			if (temp[0]!='\0') {
				strcat(temp,"\n");
				swrite(NULL,u,temp);
			}	
		}
	}

	FREE(uroom);
}

/* check roomname is a valid sized string before calling or it all may
** go pear shaped.
*/
int create_room(char *roomname)
{
	unsigned long *obj=NULL;
	int ret=-1;
	char temp[4096];

	obj=db_get_objid(roomname, OB_ROOM);

	if (!obj) {
		obj=db_set_newobj(roomname, OB_ROOM);

		if (!obj) return ret;
		ret=0;
		sprintf(temp,"room (%s) created.",roomname);
		debug(temp);
	}

	FREE(obj);
	return ret;
}

/* this will return the name of the users room
** if the room no longer exists then this will try and move the user to main
** if main doesn't exist, then it will be created.
** this should always return properly!
*/
char *get_user_room(connection *u)
{
	char *uroom=NULL;
	char buf[4096];
	unsigned long *obj=NULL;

	debug("get_user_room");

	uroom=get_attribute(u->object, "room");
	debug("1");

	if (!uroom) {
		/* must be a new user */
		uroom=MALLOC(8*sizeof(char),"get_user_room:uroom");
		sprintf(uroom,"%s","main");
	}

	obj=db_get_objid(uroom, OB_ROOM);
	debug("2");

	if (!obj) {
		/* room is missing */
		swrite(NULL,u,"The room you are in has been removed, moving to main.\n");
		if (strcmp(uroom,"main")!=0) {
			sprintf(uroom,"%s","main");
		}
		obj=db_get_objid(uroom, OB_ROOM);
	debug("3");
		if (!obj) {
			/* no main, bummer */
			if (create_room("main")!=0) {
				debug("Cannot create main");
				exit(30);
			}
			obj=db_get_objid(uroom, OB_ROOM);
	debug("4");
		}
	}

	set_attribute(u->object,"room",uroom); /* keep upto date */
	debug("9");
	FREE(obj);

	sprintf(buf,"Room - (%s) -\n",uroom);
	debug(buf);
	debug("get_user_room--");
	return uroom; /* will need FREEing by caller */
}

