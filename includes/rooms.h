/* $Id: rooms.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _ROOMS_H_
#define _ROOMS_H_

#include "socket.h" /* needed here for connection defs */

extern char *get_user_room(connection *c);
extern void look(connection *u, char *arg);
extern int create_room(char *roomname);

#endif
