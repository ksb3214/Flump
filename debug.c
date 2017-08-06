/* $Id: debug.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <flump.h>

short do_debug;

void debug(char *txt)
{
	if(do_debug==1)
	{
		printf("%s\n",txt);
	}
}
