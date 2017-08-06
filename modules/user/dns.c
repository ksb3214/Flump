/* $Id: dns.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include "flump.h"

/*
void name_get_func(connection *u)
{
	unsigned long addr;
	struct hostent *hp;
	struct sockaddr_in address;
	int j;
	char add[1024];


        j = sizeof(address);
        if(getpeername(u->fd, (struct sockaddr *) &address, &j)) {
                return;
        }
        sprintf(add,"%s",(char *)inet_ntoa(address.sin_addr));


	if((addr=inet_addr(add))==-1)
	{
		set_attribute(u->object,"dns",add);
		return;
	}
	
	hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
	if(hp == (struct hostent *)0)
	{
		set_attribute(u->object,"dns",add);
		return;
	}
	sprintf(add,"%s",hp->h_name);
	set_attribute(u->object,"dns",add);
	return;
}

void event(connection *u,short id, char *passed, char **ret)
{
	pthread_t child;
	if((id==E_CONNECT) || (id==E_RECONNECT))
	{
		pthread_create(&child,NULL,(void *)&name_get_func, u);
	}
}
*/

char *version()
{
	return "Threaded DNS module (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
