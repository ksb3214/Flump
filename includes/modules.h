/* $Id: modules.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _MODULES_H_
#define _MODULES_H_

extern void modlist(connection *u,char *arg);
extern void modload(connection *u, char *arg);
extern int modunload(connection *u, char *arg);
extern void modreloadall(connection *u, char *arg);
extern unsigned long register_module(char *name);

#endif
