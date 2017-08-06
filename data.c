#include "flump.h"

struct data_st *datatree;

struct data_st *register_data(char *facility,void *function,int public)
{
	struct data_st *sweep;

	if(!datatree)
	{
		datatree = malloc(sizeof(struct data_st));
		sweep = datatree;
	} else {
		for(sweep = datatree; sweep->next; sweep = sweep->next);
		sweep->next = malloc(sizeof(struct data_st));
		sweep = sweep->next;
	}
	sweep->facility = strdup(facility);
	sweep->function = function;
	sweep->public = public;
	sweep->next = NULL;
	return sweep;
}

void unregister_data(struct data_st *callback)
{
	struct data_st *sweep;

	if(datatree == callback)
	{
		datatree = callback->next;
		free(callback->facility);
		free(callback);	
	} else {
		for(sweep=datatree; sweep; sweep=sweep->next)
		{
			if(sweep->next == callback)
			{
				sweep->next = callback->next;
				free(callback->facility);
				free(callback);	
			}
		}
	}
}

void execute_data(connection *u, unsigned long destination, char *facility, int public)
{
	struct data_st *sweep;

	for(sweep=datatree; sweep; sweep=sweep->next)
	{
		if(!strcmp(facility,sweep->facility))
		{
			if (!sweep->function) continue;
			if((sweep->public==DATA_PUBLIC && public==DATA_PUBLIC) || (public==DATA_PRIVATE))
			{
				sweep->function(u,destination);
			}
		}
	}
}
	
