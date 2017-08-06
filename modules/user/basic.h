#ifndef _BASIC_H_
#define _BASIC_H_

struct multi_st {
	char *names;
	time_t est;
	int index;
	struct multi_st *next;
};

#endif
