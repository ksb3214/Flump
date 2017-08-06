/* $Id: commands.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <flump.h>

#define SWITCH_MODE (void *)-1
#define SWITCH_ROOT (void *)-2

command *commands;
extern lib *libs;

void listcommands(connection *u, char *arg)
{
	char temp[TMP_SIZE];
	char f[TMP_SIZE];
	command *sortit[2000]; /* very evil */
	int count=0;
	int chg=1;
	int i=0;
	int some=0;
	command *tmpint=NULL;
	command *sweep=NULL;

	format_bar(u, NULL,temp);
	swrite(NULL,u,temp);
	for(sweep=commands; sweep; sweep=sweep->next) {
		if (accessok(sweep,u) && sweep->commandmode==u->usermode) {
			sortit[i]=sweep;
			i++;
		}	
	}	
	i--;
	
        while(chg) {
                chg=0;
                for (count=0;count<=i-1;count++)
                {
                        if (strcmp(sortit[count]->name,sortit[count+1]->name)>0)
                        {
                                tmpint=sortit[count];
                                sortit[count]=sortit[count+1];
                                sortit[count+1]=tmpint;
                                chg=1;
                        }
                }
        }

        temp[0]='\0';
	some=0;
        for (count=0;count<=i;count++)
        {
		if (some) {
			strcat(temp," ");
		}	
		some=1;
		strcat(temp,sortit[count]->name);
		if (count<i) {
			if (strlen(sortit[count+1]->name)+strlen(temp)>75) {
				strcat(temp, "\n");
				swrite(NULL,u,temp);
				temp[0]='\0';
				some=0;
			}
		}	
        }
	if (temp[0]!='\0') {
		strcat(temp, "\n");
		swrite(NULL,u,temp);
	}
	sprintf(f,"%d commands",i);
	format_bar(u, f,temp);
	swrite(NULL,u,temp);
}


int is_command(char *cmd)
{
	command *sweep=NULL;

	for(sweep=commands; sweep; sweep=sweep->next)
	{
		if(!strncasecmp(sweep->name,cmd,strlen(cmd)))
		{
			return 1;
		}
	}

	return 0;
}

/* looks for priv_match(u, cmd); in modules, if it doesnt find it
** it returns a match (no privs)
*/
int accessok(command *cmd, connection *u)
{
	int (*func)(connection *u, command *cmd);

	if (strlen(cmd->name)==1) return 1;

	func=dlsym(RTLD_DEFAULT, "priv_match");

	if (!func) return 1;

	return func(u, cmd);
}

void run_command(connection *u, char *cmd2)
{
	char *verb=NULL;
	char *arg=NULL;
	char *t2=NULL;
	command *sweep=NULL;
	int found = 0;
	char *temp=NULL;
	char *cmd=NULL;
	char *endsp=NULL;
	char arg2[TMP_SIZE];

	if (!cmd2) return; /* unlikely */
	temp=MALLOC(TMP_SIZE,"run_command:temp");
	t2 = MALLOC(strlen(cmd2)+2,"run_command:t2");

	for (cmd=cmd2;*cmd==' ';cmd++);
	if(strlen(cmd)>0)
	{
		set_attribute(u->object,"idlemsg","");
		verb=cmd;
		for(sweep=commands; sweep; sweep=sweep->next)
		{
			if (sweep->commandmode!=u->usermode) continue;
			if (sweep->function) continue; /* aliases only */
			if (!sweep->alias) continue; /* silly */

			if(!strncasecmp(verb,sweep->name,strlen(sweep->name)))
			{
				verb=sweep->alias;
				arg=cmd+strlen(sweep->name);
				break;
			}
		}	

		if (!arg) {
			arg=strchr(cmd,' ');
			if (arg) {
				*arg='\0';
				arg++;
			}	
		}

		if (arg) {
			for (;*arg==' ';arg++);
			if (arg[0]=='\0') arg=NULL;
		}	

		if (arg) {
			endsp=strchr(arg,'\0');
			if (endsp) {
				endsp--;
				for (;*endsp==' ';endsp--) {
					*endsp='\0';
				}
			}
		}	

		for(sweep=commands; sweep; sweep=sweep->next)
		{
			if (sweep->commandmode!=u->usermode) continue;
			if (!sweep->function) continue;
			if (strlen(verb)>strlen(sweep->name)) continue;
			if (!strncasecmp(sweep->name,verb,strlen(verb)))
			{
				if (get_command_flag(sweep->name, C_NOSHORT)!=0) {
					if (strcasecmp(sweep->name,verb)!=0) {
						continue;
					}
				}	
				if (accessok(sweep, u)) {
					u->this_command=sweep->name;
					arg2[0]='\0';
					if (arg) {
						if (strlen(arg)<TMP_SIZE-1) {
							strcpy(arg2,arg);
						}
					}	
					sweep->function(u,arg);
					found=1;

					/* horrid kludge */
					if (strcasecmp(sweep->name,"repeat")==0) {
						break;
					}	
					u->prev_command=sweep;
					if (arg) {
						u->lastarg=(char *)realloc(u->lastarg, strlen(arg2)+2);
						strcpy(u->lastarg,arg2);
					}
					else
					{
						if (u->lastarg) FREE(u->lastarg);
						u->lastarg=NULL;
					}	
						
					break;
				}
			}	
		}

		if(!found)
		{
			sprintf(temp,"%s\n",config_get("no_command"));
			swrite(NULL,u,temp);
		}
	}
	if(u->usermode==NULL)
	{
		showprompt(u);
	} else {
		doprompt(u,u->usermode->prompt);
	}
	if (u->quiet==0) raise_event(u,E_POSTCOMMAND,verb,NULL);
	FREE(temp);
	FREE(t2);
}

void remove_command(char *name, mode *cmode)
{
	command *sweep=NULL;
	command *last=NULL;

	for(sweep=commands; sweep; sweep=sweep->next)
	{
		if((!strcasecmp(sweep->name,name)) && (sweep->commandmode==cmode))
		{
			fprintf(stderr,"Removing command %s\n",name);
			if (sweep==commands) {
				commands=sweep->next;
				last=commands;
			}
			else
			{
				last->next=sweep->next;
			}	
			FREE(sweep->name);
			if (sweep->alias) FREE(sweep->alias);
			FREE(sweep);
			sweep=last;
		}
		last=sweep;
	}  
}

/* if function is NULL then it is an alias 
** NO NESTED aliases, it is un-needed
*/
void add_command(char *name, void (*function)(connection *u,char *arg), char *alias, mode *cmode)
{
	command *sweep=NULL;
	int count=0;

	if(!commands)
	{
		commands = MALLOC(sizeof(command),"add_command:commands");
		commands->name = MALLOC(strlen(name)+2,"add_command:commands->name");
		strcpy(commands->name,name);
		commands->function = function;
		commands->alias=NULL;
		commands->commandmode=NULL;
		if (cmode) {
			commands->commandmode=cmode;
		}	
		if (alias) {
			commands->alias=MALLOC(strlen(alias)+2,"add_command:commands->alias");
			strcpy(commands->alias,alias);
		}	
		for (count=0;count<255;count++) commands->flags[count]=0;
		commands->next=NULL;
	} else {
		for(sweep=commands; sweep->next; sweep=sweep->next);
		sweep->next = MALLOC(sizeof(command),"add_command:sweep->next");
		sweep = sweep->next;
		sweep->name = MALLOC(strlen(name)+2,"add_command:sweep->name");
		strcpy(sweep->name,name);
		sweep->function = function;
		sweep->alias=NULL;
		sweep->commandmode=NULL;
		if (cmode) {
			sweep->commandmode=cmode;
		}	
		if (alias) {
			sweep->alias=MALLOC(strlen(alias)+2,"add_command:sweep->alias");
			strcpy(sweep->alias,alias);
		}	
		for (count=0;count<255;count++) sweep->flags[count]=0;
		sweep->next=NULL;
	}
}

void set_command_flag(char *name, unsigned char flag)
{
	command *sweep=NULL;

	for(sweep=commands; sweep; sweep=sweep->next) {
		if (strcasecmp(sweep->name,name)==0) {
			sweep->flags[flag]=1;
			return;
		}
	}	
}

char get_command_flag(char *name, unsigned char flag)
{
	command *sweep=NULL;

	for(sweep=commands; sweep; sweep=sweep->next) {
		if (strcasecmp(sweep->name,name)==0) {
			return sweep->flags[flag];
		}
	}	
	return 0;
}

void clear_commands()
{
	command *sweep=NULL;
	command *next=NULL;

	for(sweep=commands; sweep; sweep=next)
	{
		next=sweep->next;
		FREE(sweep->name);
		if (sweep->alias) FREE(sweep->alias);
		FREE(sweep);
	}
}

/* well if i do this correctly then we should be able to do some funky stuff
** with events, such as command privs
**
** passed is info on which the event handlers may depend.
** ret is passed back and will need FREEing by the caller
*/
void raise_event(connection *u, short id, char *passed, char **ret)
{
	lib *sweep=NULL;

	for(sweep=libs; sweep; sweep=sweep->next)
	{
		if(sweep->event)
		{
			sweep->event(u,id,passed,ret);
		}
	}
}
