/* $Id: config.c,v 2.1 2005/07/07 16:27:09 ksb Exp $ */
/* $Revision: 2.1 $ */

#include <flump.h>

char conf_name[300][100];
char conf_data[300][1024];
char config_file[1024];

struct menu_s {
	char *variable;
	char *name;
	char *defval;
};

struct menu_s c_database[] = {
	{"db_host","Database host","localhost"},
	{"db_name","Database name","flump"},
	{"db_user","Username","flump"},
	{"db_pass","Password","flump"},
	{"db_objects","Objects table","objects"},
	{"db_attributes","Attributes table","attributes"},
	{"","",""}
};

struct menu_s c_prompts[] = {
	{"prompt_login","Login prompt","Login:"},
	{"prompt_pass","Password prompt","Password:"},
	{"prompt_newpass1","First new password prompt","New password:"},
	{"prompt_newpass2","Second new password prompt","Repeat password:"},
	{"prompt_default","Default prompt","flump>"},
	{"","",""}
};

struct menu_s c_customise[] = {
	{"talker_name","Name of the talker","Flump"},
	{"new_welcome","New user welcome message","Welcome, new user"},
	{"no_command","Unknown command message","What?"},
	{"format_bar","Pretty formatting bar","=============================================================================="},
	{"","",""}
};

struct menu_s c_cache[] = {
	{"cache_max","Max cache blocks","100"},
	{"","",""}
};

struct menu_s c_network[] = {
	{"ip_port","Main IP port","5000"},
	{"","",""}
};

struct menu_s c_modules[] = {
	{"modules","Auto-loaded modules","modules/core/login.so,modules/user/basic.so,modules/user/admin.so,modules/user/rooms.so,modules/user/privs.so,modules/user/info.so,modules/user/custom.so,modules/user/auto.so,modules/user/spodtimes.so,modules/user/lists.so,modules/user/socials.so"},
	{"","",""}
};

const char opts[] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int confcount;

int load_config(char *filename)
{
	FILE *conf;
	char buffer[4096];
	char *scan;
	char *dat;
	int linecount=0;
	confcount=0;

	conf = fopen(filename,"r");
	if(!conf)
	{
		fprintf(stderr,"Cannot open config file: %s\n",filename);
		return 0;
	}
	while(fgets(buffer,4095,conf))
	{
		linecount++;
		scan = buffer;
		while((*scan<=' ') && (scan<(buffer+strlen(buffer))))
			scan++;
		if(strlen(scan)>0)
		{
			dat=(scan+strlen(scan));
			while((*dat<=' ') && (dat>scan))
			{
				*dat=0; 
				dat--;
			}
		}
		if((*scan!='#') && (strlen(scan)>0))
		{
			dat=scan;
			while((*dat!=' ') && (dat<(scan+strlen(scan))))
				dat++; 
			if(dat==(scan+strlen(scan)))
			{
				fprintf(stderr,"Syntax error in %s at line %d\n",
					filename,linecount);
				exit(10);
			}
			*dat=0;
			dat++;
			sprintf(conf_name[confcount],scan);
			sprintf(conf_data[confcount],dat);
			confcount++;
		}
	}
	fclose(conf);
	return 1;
}

void config_set(char *key, char *val)
{
	int i;
	for(i=0; i<confcount; i++)
	{
		if(!strcasecmp(conf_name[i],key))
		{
			sprintf(conf_data[i],"%s",val);
			return;
		}
	}
	sprintf(conf_name[confcount],"%s",key);
	sprintf(conf_data[confcount],"%s",val);
	printf("%d : %s - %s\n",confcount,key,val);
	confcount++;
}

char *config_get(char *key)
{
	int sweep;

	for(sweep=0; strlen(conf_name[sweep])>0; sweep++)
	{
		if(!strcasecmp(conf_name[sweep],key))
		{
			return conf_data[sweep];
		}
	}
	printf("No config for (%s).\n",key);
	return "NO CONFIG";
}

int match(char v,char *str)
{
	int i;
	for(i=0; i<strlen(str); i++)
	{
		if(str[i]==v)
		{
			return 1;
		}
	}
	return 0;
}

char main_menu()
{
	char temp[TMP_SIZE];

	temp[0]=0; temp[1]=0;
	while(!match(temp[0],"123456QE"))
	{
		printf("\n");
		printf("    Main Menu\n");
		printf("    =========\n\n");
		printf("    1) Database\n");
		printf("    2) Prompts\n");
		printf("    3) Customisations\n");
		printf("    4) Cache\n");
		printf("    5) Network\n");
		printf("    6) Modules\n");
		printf("    Q) Exit without saving\n");
		printf("    E) Save and Exit\n\n");
		printf("    Enter your choice: ");
		fgets(temp,1024,stdin);
		if(temp[0]>='a' && temp[0]<='z')
		{
			temp[0]=temp[0]-32;
		}
	}
	return temp[0];
}

void run_menu(struct menu_s *m)
{
	int i;
	char ms[64];
	char temp[TMP_SIZE];

	temp[0]=0; temp[1]=0;
	ms[0]='0';
	for(i=0; strlen(m[i].variable); i++)
	{
		ms[i+1] = opts[i];
	}
	while(temp[0]!='0')
	{
		while(!match(temp[0],ms))
		{
			printf("\n\n");
			for(i=0; strlen(m[i].variable); i++)
			{
				printf("    %c) %-30s [%-40s]\n",opts[i],m[i].name,config_get(m[i].variable));
				ms[i+1] = opts[i];
			}
			printf("\n");
			printf("    0) Return to previous menu\n\n");
			printf("    Enter your choice: ");
			fgets(temp,1024,stdin);
			if(temp[0]>='a' && temp[0]<='z')
			{       
				temp[0]=temp[0]-32;
			}
		}
		for(i=0; i<strlen(ms); i++)
		{
			if(ms[i+1]==temp[0])
			{
				printf("Setting: %s\n",m[i].name);
				printf("Current value: %s\n",config_get(m[i].variable));
				printf("New value [return to keep old]: ");
				fgets(temp,1024,stdin);
				temp[strlen(temp)-1]=0;
				if(temp[0]!=0)
				{
					config_set(m[i].variable,temp);
				}
				temp[0]=0; temp[1]=0;
				i=strlen(ms);
			}
		}
	}
	printf("Dropped out\n");

}

void reconfigure()
{
	char c;
	FILE *conf;
	int i;
	

	while(1)
	{
		c=main_menu();
		switch(c)
		{
			case 'E':
				conf = fopen(config_file,"w");
				for(i=0; i<confcount; i++)
				{
					fprintf(conf,"%s %s\n",conf_name[i] ,conf_data[i]);
				}
				fclose(conf);
				exit(0);
				break;
			case 'Q':
				exit(0);
				break;
			case '1':
				run_menu(c_database);
				break;
			case '2':
				run_menu(c_prompts);
				break;
			case '3':
				run_menu(c_customise);
				break;
			case '4':
				run_menu(c_cache);
				break;
			case '5':
				run_menu(c_network);
				break;
			case '6':
				run_menu(c_modules);
				break;
		}
	}
}

void set_default(struct menu_s *m)
{
	int ci;
	for(ci=0; strlen(m[ci].variable); ci++)
	{
		config_set(m[ci].variable,m[ci].defval); 
	}
	printf("--\n");
}

void set_all_defaults()
{
	set_default(c_database);
	set_default(c_prompts);
	set_default(c_customise);
	set_default(c_network);
	set_default(c_cache);
}


