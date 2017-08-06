/* $Id: adminport.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <pthread.h>
#include "flump.h"

pthread_t socketthread;
int pth_terminate=0;

static int fd;
void adminsocket(char *moo)
{
        struct sockaddr_in main_socket;
	char buffer[1024];
	int on=1;

        bzero((u_char *) &main_socket, sizeof(struct sockaddr_in));
        main_socket.sin_family = AF_INET;
        main_socket.sin_port = htons(atoi(config_get("admin_port")));
        main_socket.sin_addr.s_addr=INADDR_ANY;
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
        {
                fprintf(stderr,"Couldn't grab the socket!!!\n");
                return;
        }

        setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));
        if (bind(fd, (struct sockaddr *) &main_socket, sizeof(main_socket)) < 0)
        {
                fprintf(stderr,"Cannot bind.\n");
		return;
        }
        if(listen(fd,10)<0)
        {
                fprintf(stderr,"Listen refused.\n");
		return;
        }

	while(pth_terminate==0)
	{
	        struct sockaddr_in newsock;
		char *a1=NULL,*a2=NULL;
        	int len;
		int lfd;
		int i,j;

		len=sizeof(newsock);

		bzero(buffer,1024);
        	lfd=accept(fd,(struct sockaddr *)&newsock,&len);
		read(lfd,buffer,1024);
		buffer[strlen(buffer)-1]=0;
		j = strlen(buffer);
		for(i=0; i<j; i++)
		{
			if(buffer[i]==' ' && !a1)
			{
				buffer[i]=0;
				a1=buffer+i+1;
			} else {
				if(buffer[i]==' ' && !a2)
				{
					buffer[i]=0;
					a2=buffer+i+1;
				}
			}
		}
		if(!strcasecmp(buffer,"get_attr") && a1 && a2)
		{
			char *att;
			att=get_attribute(atol(a1),a2);
			if(att)
				write(lfd,att,strlen(att));
			write(lfd,"\n",1);
			FREE(att);
		}
		close(lfd);
	}
	close(fd);
	return;
}

int init()
{
	int i;
	i=pthread_create(&socketthread,NULL,(void *)&adminsocket,"");
	if(i!=0)
	{
		printf("Error starting thread: %d\n",i);
	}
	return 1;
}

int uninit()
{
	int i;
	pth_terminate=1;
	i=pthread_join(socketthread,0);
	if(i!=0)
	{
		fprintf(stderr,"Error joining thread: %d\n",i);
	}
	return 1;
}

char *version()
{
	return "Administrative control port module (c) Karl Bastiman $Revision: 2.0 $";
}
