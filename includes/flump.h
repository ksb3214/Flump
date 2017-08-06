/* $Id: flump.h,v 2.1 2005/07/08 10:01:39 ksb Exp $ */
/* $Revision: 2.1 $ */

#ifndef _FLUMP_H_
#define _FLUMP_H_

/* #define DEBUGMEM	1 */
#define _GNU_SOURCE	1

#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif 

#include <defs.h>

#if STDC_HEADERS
# include <stdio.h>
#endif
#if HAVE_STDLIB_H
# include <stdlib.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_CTYPE_H
# include <ctype.h>
#endif
#if HAVE_DLFCN_H
# include <dlfcn.h>
#endif
#if HAVE_ERRNO_H
# include <errno.h>
#endif
#if HAVE_SIGNAL_H
# include <signal.h>
#endif
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_STRING_H
# include <string.h>
#else
# if HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif
#if HAVE_MYSQL_MYSQL_H
# include <mysql/mysql.h>
#endif
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif
#if HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#if HAVE_ARPA_TELNET_H
# include <arpa/telnet.h>
#endif
#if HAVE_NETDB_H
# include <netdb.h>
#endif
#if HAVE_PTHREAD_H
# include <pthread.h>
#endif

#define TMP_SIZE	4096

#include "globals.h"
#include "signals.h"
#include "database.h"
#include "cache.h"
#include "config.h"
#include "rooms.h"
#include "commands.h"
#include "debug.h"
#include "events.h"
#include "malloc.h"
#include "socket.h"
#include "formatting.h"
#include "misc.h"
#include "modules.h"
#include "data.h"

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

#ifndef bzero
extern void bzero(void *, size_t);
#endif

/* object types */
#define OB_USER	1
#define OB_ROOM	2
#define OB_PRIV 3
#define OB_COMMAND 4
#define OB_SOCIAL 5
#define OB_MODULE 6
#define OB_HELP 7
#define OB_NEWS 8

#endif
