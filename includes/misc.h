/* $Id: misc.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _MISC_H_
#define _MISC_H_

extern int iscolour(char);
extern void stolower(char *str);
extern void insert_into(char **buf, char *what);
extern void remove_from(char **buf, char *what);
extern int in_delim(char *buf, char *what);
extern void doprompt(connection *c, char *str); 
connection *getubyname(connection *u, char *name, char *errormsg);
extern char *getgen(connection *, char *, char *, char *);
extern void echo_on(connection *);
extern void echo_off(connection *);
extern char *timeval(time_t num);
extern char *daysago(time_t num);

extern char retval[TMP_SIZE];
#define PR_OPEN 0
#define PR_CLOSED 1

#endif
