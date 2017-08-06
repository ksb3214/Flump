/* $Id: cache.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _CACHE_H_
#define _CACHE_H_

#include <time.h>

struct cache_s {
	unsigned long object;
	char *name;
	char *value;
	time_t timestamp;
	struct cache_s *next;
};
typedef struct cache_s cache;

extern short maxcache;

extern char *get_attribute(unsigned long, char *);
extern void set_attribute(unsigned long, char *, char *);
extern void cache_clear();
extern void delete_attribute(unsigned long, char *);
extern void delete_all_attributes(unsigned long);

#endif
