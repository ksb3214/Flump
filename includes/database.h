/* $Id: database.h,v 2.2 2005/07/07 11:42:43 ksb Exp $ */
/* $Revision: 2.2 $ */

#ifndef _DATABASE_H_
#define _DATABASE_H_

struct lists_st {
	unsigned long oid;
	char *value;
	struct lists_st *next;
};

extern void init_db();
extern char *db_get_obj_name(unsigned long);
extern char *db_get_attribute(unsigned long, char *);
extern void db_set_attribute(unsigned long, char *, char *);
extern void db_delete_obj(unsigned long id);
extern unsigned long *db_get_objid(char *name, unsigned long type);
extern unsigned long *db_set_newobj(char *name, unsigned long type);
extern char *config_get(char *);
extern void db_delete_attribute(unsigned long, char *);
extern void db_delete_all_attributes(unsigned long);
extern void freelist(struct lists_st *lst);
extern struct lists_st *getvalues(int type, char *attr2);
void MYSQL_FREE(MYSQL_RES *var);

#endif
