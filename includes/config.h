/* $Id: config.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _CONFIG_H_
#define _CONFIG_H_

extern int load_config(char *);
extern char *config_get(char *);
extern void reconfigure();
extern char config_file[];
extern void set_all_defaults();

#endif
