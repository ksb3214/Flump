/* $Id: main.c,v 2.2 2005/07/07 16:27:09 ksb Exp $ */
/* $Revision: 2.2 $ */

#include <flump.h>

int reconf;
struct global_s global;

/* only enable this is you want it to cresh out on any memory error 
*/
/* extern char *malloc_options;
malloc_options = "A";
*/


void look(connection *u, char *arg);

void usage()
{
	fprintf(stdout,"Usage: flump [options]\n");
	fprintf(stdout,"	-h 		This help information\n");
	fprintf(stdout,"	-f <filename>	Use alternate config file\n");
	fprintf(stdout,"	-c 		Interactive reconfiguration\n");
}

void run()
{
	connection *sweep=NULL;
	lib *sweep2=NULL;
	struct timeval timeout;
	int ready;
	fd_set fdset;
	unsigned long obj=0;
	connection *next=NULL;

	global.logins_since=0;
	global.running=1;
	timeout.tv_sec=1;
	timeout.tv_usec=0;
	while(global.running)
	{
		FD_ZERO(&fdset);
		FD_SET(main_descriptor,&fdset);
		for(sweep=users; sweep; sweep=sweep->next)
		{
			FD_SET(sweep->fd,&fdset);
		}

		ready=select(FD_SETSIZE,&fdset,0,0,&timeout);
		if(ready==-1)
		{
			fprintf(stderr,"select() error: %d\n",errno);
			continue;
			/* exit(20); */
		}
		else
/*		if(ready>0) */
		{
			if(FD_ISSET(main_descriptor,&fdset))
			{
				accept_connection();
			}
			for(sweep=users; sweep; sweep=next)
			{
				next=sweep->next; /* just incase of disconnect */
				if(FD_ISSET(sweep->fd,&fdset))
				{
					if (input(sweep)) {
						sweep->function(sweep,sweep->buffer);
					}	
				}
			} 
			for(sweep=users; sweep; sweep=next)
			{
				next=sweep->next; /* just incase of disconnect */
				if (sweep->destroy==1) {
					obj=sweep->object;
					disconnect(sweep);
					db_delete_obj(obj);
					continue;
				}	
				if (sweep->disconnect==1) {
					disconnect(sweep);
					continue;
				}	
			} 
		}		
		for(sweep2=libs; sweep2; sweep2=sweep2->next)
		{
			if(sweep2->tick)
			{
				sweep2->tick();
			}
		}
	}
}

void setup()
{
	char *modules = NULL;	
	char *mod = NULL;
	char *pos = NULL;
	init_sig();

	if(!load_config(config_file))
	{
		set_all_defaults();
		printf("No config file found - defaults loaded.\n");
		reconf=1;
	}

	if(reconf)
	{
		reconfigure(); 
		exit(0);
	}

	maxcache=atoi(config_get("cache_max"));
	do_debug=atoi(config_get("debug")); 
	init_db();
	init_main_socket();
	add_command("look", &look,NULL,NULL);
	add_command("modlist",&modlist,NULL,NULL);
	add_command("modload",&modload,NULL,NULL);
	add_command("modunload",(void *)&modunload,NULL,NULL);
	add_command("modreloadall",&modreloadall,NULL,NULL);

	modules = config_get("modules");
	for(mod = strtok_r(modules,",",&pos); mod; mod=strtok_r(NULL,",",&pos))
		load_dll(mod); 
}

void cleanup()
{
	clear_commands();
	clear_dlls();
	close_connections();
	cache_clear();
#ifdef DEBUGMEM
	memreport();
#endif
	printf("Memory deficit: %lu",memory_usage);
}

int main(int argc, char **argv)
{
	int ch;

	global.boot_time=time(NULL);

	reconf=0;
	sprintf(config_file,"%s%s",getenv("HOME"),"/.flumprc");

	while ((ch = getopt(argc, argv, "cf:h")) != -1)
	{
		switch (ch) 
		{
			case 'f':
				if(!optarg)
				{
					usage();
					exit(0);
				}
				sprintf(config_file,"%s",optarg);
				break;
			case 'c':
				reconf=1;
				break;
			case 'h':
				usage();
				exit(0);
				break;
			default:
				usage();
				exit(0);
				break;
		}
	}
	setup();
	run(); 
	cleanup();
	return 0;
}
