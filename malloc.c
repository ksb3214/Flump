/* $Id: malloc.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <flump.h>

long memory_usage;

#ifdef DEBUGMEM
struct meminfo {
	char *name;
	long all;
	void *ptr;
	struct meminfo *next;
};

struct meminfo *cmem=NULL;

void memadj(void *ptr, char *desc, int adj)
{
	struct meminfo *sweep=NULL;
	int fnd=0;

	if (adj<1) {
		for (sweep=cmem;sweep;sweep=sweep->next) {
			if (sweep->ptr==ptr) {
				sweep->all+=adj;
				sweep->ptr=NULL;
				fnd=1;
			}
		}
	}	

	if (!fnd) {
		if (cmem) {
			for (sweep=cmem;sweep->next;sweep=sweep->next);
			sweep->next=malloc(sizeof(struct meminfo));
			sweep=sweep->next;
		}
		else
		{
			cmem=malloc(sizeof(struct meminfo));
			sweep=cmem;
		}

		sweep->all=adj;
		sweep->ptr=ptr;
		sweep->name=malloc(strlen(desc)+4);
		strcpy(sweep->name,desc);
		sweep->next=NULL;
	}	
		
}

void memreport()
{
	struct meminfo *sweep=NULL;

	for (sweep=cmem;sweep;sweep=sweep->next) {
		if (sweep->all!=0) {
			printf("(%s) ... (%ld)\n",sweep->name,sweep->all);
		}	
	}
}

#endif

void *MALLOC(size_t s, char *desc)
{
	void *ptr=NULL;

	ptr=malloc(s);
#ifdef DEBUGMEM
	memadj(ptr,desc,1);	
#endif
	memory_usage++;
	return ptr;
}

void FREE(void *s)
{
	if (s) {
#ifdef DEBUGMEM
		memadj(s,"Unallocated",-1);
#endif
		free(s);
		memory_usage--;
	}
}

void MYSQL_FREE(MYSQL_RES *var)
{
	if (!var) return;
	mysql_free_result(var);
}	
