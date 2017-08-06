/* $Id: auto.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

/*******************************************************
 * auto.c - timed messages and such                    *
 *******************************************************
 * Currently this module just handles the clock.       *
 *******************************************************/

#include <time.h>
#include "flump.h"

#define MESS_DEFAULT ">> You suddenly realise it's %d o'clock and you're still idle.\n"

unsigned long myobject=0;

short hour;	/* Stores the current hour */
char *mess;
char message[TMP_SIZE];

/* Muffle the clock for one user */

void set_clock(connection *u,char *m)
{
	char temp[TMP_SIZE];
	char *scan;
	int dcount=0;
	int ocount=0;

	if (!m) {
		swrite(NULL,u,"Set the message to what?\n");
		return;
	}

	for(scan=m; *(scan+1); scan++)
	{
		if(*scan=='%')
		{
			switch (*(scan+1))
			{
				case 'd': 
					dcount++;
					break;
				default:
					ocount++;
					break;
			}
		}
	}
	
	if((dcount!=1) || (ocount!=0))
	{
		swrite(NULL,u,"You must have only ONE %d in the clock message.\n");
		return;
	}
	
	sprintf(message,"%s\n",m);
	set_attribute(myobject,"message",message);
	swrite(NULL,u,"Clock message set to:\n");
	sprintf(temp,message,hour);
	swrite(NULL,u,temp);
}

/* Test the time and if the hour by the clock is different from the
 * recorded hour broadcast the time to all those as want it.  Store
 * the new hour for compating later. */

void run_clock()
{
	char temp[1024];
	time_t t;
	struct tm *lt;
	connection *sweep;
	char *muff=NULL;
	int m=0;
	int mhour;

	t=time(NULL);
	lt=localtime(&t);
	if(lt->tm_hour!=hour)
	{ 
		hour=lt->tm_hour; 
		mhour=hour%12;
		if(mhour==0) mhour=12;
		sprintf(temp,message,mhour);
		for(sweep=users; sweep; sweep=sweep->next)
		{
			m=0;
			muff=get_attribute(sweep->object,"muffle_clock");
			if(muff)
			{
				if(*muff=='Y')
				{
					m=1;
				}
				FREE(muff);
			}
			if(!m && !sweep->quiet)
				swrite(NULL,sweep,temp);
		}
	}
}

/* Main tick stub */

void tick()
{
	run_clock();	/* Do the clock tests */
}

int init()
{
	time_t t;
	struct tm *lt;

	t=time(NULL);
	lt=localtime(&t);
	hour=lt->tm_hour; 

	myobject = register_module("auto");
	
	if(myobject==0)
	{
		return 0;
	}

	mess = get_attribute(myobject,"message");
	
	if(!mess)
	{
		set_attribute(myobject,"message",MESS_DEFAULT);
		mess = get_attribute(myobject,"message");
	}
	sprintf(message,"%s",mess);
	FREE(mess);

	add_command("set_clock",&set_clock,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("set_clock",NULL);
	return 1;
}

char *version()
{
	return "Auto system (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
