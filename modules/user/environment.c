/* $Id: environment.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

void tick()
{
	connection *sweep;
	int i;
	char temp[TMP_SIZE];

	i=random()%200;
	if(i==1) { sprintf(temp,"Snowflakes wheel and spin around you.\n"); }
	if(i==2) { sprintf(temp,"A chill wind whistles through the trees.\n"); }
	if(i==3) { sprintf(temp,"With a rustle and thud a clump of snow drops off a nearby branch.\n"); }
	if(i==4) { sprintf(temp,"Someone screams in the distance.\n"); }
	if(i==1 || i==2 || i==3 || i==4)
	{
		for(sweep=users; sweep; sweep=sweep->next)
		{
			swrite(NULL,sweep,temp);
		}
	}
}

char *version()
{
	return "Example environmenty module (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
