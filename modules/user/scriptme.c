/* $Id: scriptme.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

int init()
{
	add_command("motddel",&motddel,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("motddel",NULL);
	return 1;
}

char *version()
{
	return "Script system (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
