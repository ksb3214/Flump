/* $Id: malloc.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _MALLOC_H_
#define _MALLOC_H_

extern long memory_usage;

extern void *MALLOC(size_t, char *);
extern void FREE(void *);
#ifdef DEBUGMEM
extern void memreport();
#endif

#endif
