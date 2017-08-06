/* $Id: globals.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _GLOBALS_H
#define _GLOBALS_H

struct global_s {
	time_t  boot_time;
	int	running;
	unsigned long logins_since;
	int 	max_on;
	MYSQL 	*database;
};

extern struct global_s global;

#endif
