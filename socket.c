/* $Id: socket.c,v 2.1 2005/07/07 16:27:09 ksb Exp $ */
/* $Revision: 2.1 $ */
/* an edit */

#include <flump.h>

int main_descriptor;
connection *users;

void showprompt(connection *u)
{
	char *prompt=NULL;

	prompt=get_attribute(u->object,"prompt");
	doprompt(u,prompt?prompt:"> ");
	if (prompt) FREE(prompt);
}

void init_main_socket()
{
        struct sockaddr_in main_socket;
	int on=1;
	int opts;

        bzero((u_char *) &main_socket, sizeof(struct sockaddr_in));
        main_socket.sin_family = AF_INET;
        main_socket.sin_port = htons(atoi(config_get("ip_port")));
	main_socket.sin_addr.s_addr=INADDR_ANY;
        main_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (main_descriptor < 0)
	{
                fprintf(stderr,"Couldn't grab the socket!!!\n");
		exit(20);
	}
	fprintf(stderr,"Got Socket...\n");

	setsockopt(main_descriptor,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));
        if (bind(main_descriptor, (struct sockaddr *) &main_socket, sizeof(main_socket)) < 0)
        {
	        fprintf(stderr,"Cannot bind. (%s)\n",config_get("ip_port"));
		exit(20);
	}
	fprintf(stderr,"Port Bound...\n");
        if(listen(main_descriptor,10)<0)
	{
                fprintf(stderr,"Listen refused.\n");
		exit(20);
	}

	fprintf(stderr,"Listen set...\n");
	opts = O_NONBLOCK;
	fcntl(main_descriptor,F_SETFL,opts);
}

void shutdown_main_socket()
{
}

/* does not obey ignore */
void swall(connection *u, char *txt)
{
	connection *sweep=NULL;

	debug("swall");
	for(sweep=users; sweep; sweep=sweep->next)
	{
		if (sweep==u) continue;
		if (sweep->quiet) continue;
		swrite(NULL,sweep,txt);
	}
	debug("swall--");
}

void sroom(connection *u, char *txt)
{
	connection *sweep=NULL;
	char *uroom=NULL;
	char *swroom=NULL;

	debug("sroom");
	for(sweep=users; sweep; sweep=sweep->next)
	{
		if (sweep==u) continue;
		if (sweep->quiet) continue;
		uroom=(char *)get_user_room(u);
		swroom=(char *)get_user_room(sweep);
		if (strcmp(uroom,swroom)==0) {
			swrite(u,sweep,txt);
		}
		FREE(uroom);
		FREE(swroom);
	}
	debug("sroom--");
}



/* Totally nicked from smud
*/
void wsock(int socket,char *str)
{
        write(socket,str,strlen(str));
}
 
/* Totally nicked from smud
*/
void wuser(connection *u, unsigned char *str)
{
	long bufpos=0;
	char *start;
	char buff[TMP_SIZE];
	short col=1;
	int sock=0;
	char *dat=NULL;
	int hitell=0;

	start=str;
	bufpos=0;
	sock=u->fd;
	/* col=UU.colour; */
	if (sock<0) return;

	dat=get_attribute(u->object,"colour");
	if (dat) {
		if (dat[0]=='N') col=0;
		FREE(dat);
	}	

	while (*str) {
		if (*str=='\n')
		{
			if (bufpos>4090) 
			{
				write(sock,buff,bufpos);
				bufpos=0;
			}

			hitell=0;
			dat=get_attribute(u->object,"hitells");
			if (dat) {
				if (dat[0]=='Y') hitell=1;
				FREE(dat);
			}	
			if (col || hitell) 
			{
				memcpy(buff+bufpos,"\033[0m",4);
				bufpos+=4;
			}			
			
			*(buff+bufpos)='\r';
			*(buff+bufpos+1)='\n';
			bufpos+=2;
			str++;
		}
		else
		{
/*			if (*str=='^' && *(str+1)=='^') 
			{
				str+=2;
				continue;
			}
*/
			if (*str=='^')
			{
				if (bufpos>4090) {
					write(sock,buff,bufpos);
					bufpos=0;
				}
				str++;
				hitell=0;
				dat=get_attribute(u->object,"hitells");
				if (dat) {
					if (dat[0]=='Y')  {
						if (*str=='*') hitell=1;
					}	
					FREE(dat);
				}	
				if ((col || hitell) || *str=='^')
				{
					switch(*str) {
						case 'r' : memcpy(buff+bufpos,"\033[0m\033[31m",9);
							   bufpos+=8;
							   break;
						case 'g' : memcpy(buff+bufpos,"\033[0m\033[32m",9);
							   bufpos+=8;
							   break;
						case 'y' : memcpy(buff+bufpos,"\033[0m\033[33m",9);
							   bufpos+=8;
							   break;
						case 'b' : memcpy(buff+bufpos,"\033[0m\033[34m",9);
							   bufpos+=8;
							   break;
						case 'p' : memcpy(buff+bufpos,"\033[0m\033[35m",9);
							   bufpos+=8;
							   break;
						case 'm' : memcpy(buff+bufpos,"\033[0m\033[35m",9);
							   bufpos+=8;
							   break;
						case 'c' : memcpy(buff+bufpos,"\033[0m\033[36m",9);
							   bufpos+=8;
							   break;
						case 'w' : memcpy(buff+bufpos,"\033[0m\033[37m",9);
							   bufpos+=8;
							   break;
						case '*' : memcpy(buff+bufpos,"\033[1m",4);
							   bufpos+=3;
							   break;
						case 'H' : memcpy(buff+bufpos,"\033[1m",4);
							   bufpos+=3;
							   break;
						case 'N' : memcpy(buff+bufpos,"\033[0m",4);
							   bufpos+=3;
							   break;
						case 'A' : memcpy(buff+bufpos,"\033[1m\033[30m",9);
							   bufpos+=8;
							   break;
						case 'R' : memcpy(buff+bufpos,"\033[1m\033[31m",9);
							   bufpos+=8;
							   break;
						case 'G' : memcpy(buff+bufpos,"\033[1m\033[32m",9);
							   bufpos+=8;
							   break;
						case 'Y' : memcpy(buff+bufpos,"\033[1m\033[33m",9);
							   bufpos+=8;
							   break;
						case 'B' : memcpy(buff+bufpos,"\033[1m\033[34m",9);
							   bufpos+=8;
							   break;
						case 'P' : memcpy(buff+bufpos,"\033[1m\033[35m",9);
							   bufpos+=8;
							   break;
						case 'M' : memcpy(buff+bufpos,"\033[1m\033[35m",9);
							   bufpos+=8;
							   break;
						case 'C' : memcpy(buff+bufpos,"\033[1m\033[36m",9);
							   bufpos+=8;
							   break;
						case 'W' : memcpy(buff+bufpos,"\033[1m\033[37m",9);
							   bufpos+=8;
							   break;
						case '^' : *(buff+bufpos)=*str;
							   break;
						default  : str--;
							   bufpos--;
							   break;
					}
				} else bufpos--; 
			}
			else
			{
				*(buff+bufpos)=*str;
			}
			bufpos++;
			str++;
		}

		if (bufpos==4090)
		{
			write(sock,buff,bufpos);
			bufpos=0;
		}
	}
	
	if (bufpos) {
		buff[bufpos]='\0';
		write(sock,buff,bufpos);
	}
}

char *adjust(connection *u, char *txt)
{
	char *search=NULL;
	char token[TMP_SIZE];
	char temp[TMP_SIZE];
	char temp2[TMP_SIZE];
	char temp3[TMP_SIZE];
	char *output=NULL;
	unsigned long *obj=NULL;
	int tn=0;
	int s2=0;
	char *cn=NULL;
	char *cap=NULL;
	int cname=1;
	char *at=NULL;
	char *sp=NULL;
	int col=1;
	char *dat=NULL;
	int expand=0;
	int len=0;
	int nextcname=0;
	connection *expu=NULL;

	output=MALLOC(TMP_SIZE*5,"adjust:output");
	output[0]='\0';
	token[0]='\0';

	if (!txt) return output;

	dat=get_attribute(u->object,"muffle_cname");
	if (dat) {
		if (dat[0]=='N') col=0;
		FREE(dat);
	}	

	for (search=txt;*search!='\0';search++) {
		sp=NULL;
		at=NULL;
		sp=strchr(search,' ');
		if (!sp) sp=strchr(search,'\n');
		at=strchr(search,'@');
		if (at&&sp) if (at<sp) cname=0;
		if (*search==' ') {
			cname=1;
		}	
		if (*search=='/') {
			cname=0;
		}	
		if (isalpha(*search)) {
			token[tn]=*search;
			tn++;
			token[tn]='\0';
			if (tn>MAX_NAME) {
				strcat(output,token);
				tn=0;
				token[0]='\0';
			}
		}
		else
		{
			if (nextcname) {
				cname=0;
				nextcname=0;
			}	
			if (tn>0) {
				if (strcasecmp(token,"www")==0) cname=0;
				if (strcasecmp(token,"http")==0) cname=0;
				if (expand) {
					expu=getubyname(u,token,temp3);
					if (expu) {
						strcpy(token,expu->name);
						len=strlen(output);
						if (len>0) {
							output[len-1]='\0';
						}	
					}
					expand=0;
				}	
				obj=db_get_objid(token,OB_USER);
				if (obj) {
					s2=0;
					temp[0]='\0';
					/* do recap */
					cap=get_attribute(*obj,"recap");
					if (cap) {
						if (cname) strcpy(token,cap);
						FREE(cap);
					}	
					cn=get_attribute(*obj,"cname");
					if (cn) {
					 	/* do cname */
						for (s2=0;cn[s2]!='\0';s2++) {
							if (cname&&col) {
								sprintf(temp2,"^%c%c^N",cn[s2],token[s2]);
							}
							else
							{
								sprintf(temp2,"%c",token[s2]);
							}	
							strcat(temp,temp2);
						}
						sprintf(temp2,"^N%c",*search);
						strcat(temp,temp2);
						FREE(cn);
					}
					else
					{
						sprintf(temp,"%s%c",token,*search);
					}	
					FREE(obj);
				}
				else
				{
					sprintf(temp,"%s%c",token,*search);
				}	
			}
			else
			{
				sprintf(temp,"%c",*search);
			}	
			tn=0;
			token[0]='\0';
			strcat(output,temp);
			expand=0;
		}
		if (*search=='$') {
			if (*(search+1)=='-') {
				len=strlen(output);
				if (len>0) {
					output[len-1]='\0';
				}	
				nextcname=1;
				search++;
			}	
			else
			{
				if (*(search+1)=='$') {
					search++;
				}	
				else
				{
					expand=1;
				}
			}	
		}	
	}
	return output;
}

/* optionally from s
** directed to u
*/
void swrite(connection *s,connection *u, char *txt)
{
	char *rev=NULL;
	char *dat=NULL;

	if (s) {
		/* check ignore */
		dat=get_attribute(u->object,"ignores");
		if (dat) {
			if (in_delim(dat,s->name)) {
				FREE(dat);
				return;
			}
			FREE(dat);
		}
	}	
	rev=adjust(u, txt);
	wuser(u, rev);
	FREE(rev);
}

void accept_connection()
{
	int nsock=0;
	struct sockaddr_in newsock;
	connection *sweep=NULL;
	int len=0;
	int count=0;
	char temp[TMP_SIZE];
		
	len=sizeof(newsock);
	if((nsock=accept(main_descriptor,(struct sockaddr *)&newsock,&len)))
	{
		if(users)
		{
			for(sweep=users; sweep->next; sweep=sweep->next);
			sweep->next = MALLOC(sizeof(connection),"accept_connection:sweep->next");
			sweep = sweep->next;
		} else {
			users = MALLOC(sizeof(connection),"accept_connection:users");
			sweep = users;
		}
		sprintf(temp,"%c%c%c",IAC,DO,TELOPT_TTYPE);
		wsock(nsock,temp);
		sprintf(temp,"%c%c%c",IAC,DO,TELOPT_NAWS);
		wsock(nsock,temp);

		sweep->fd=nsock;
		sweep->quiet=1;
		sweep->function=dlsym(RTLD_DEFAULT,"login");
		if(!sweep->function)
		{
			fprintf(stderr,"Cannot locate login() function.\n");
			exit(20);
		}
		sweep->timestamp=time(0);
		sweep->object=0;
		sweep->quickdis=1;
		sweep->disconnect=0;
		sweep->destroy=0;
		sweep->last_command=time(0);
		sweep->logged_in=time(0);
		sweep->last_spod=time(0);
		sweep->last_sync=time(0);
		strcpy(sweep->ttype, "Default (no detect)");
		sweep->twidth=80;
		sweep->theight=25;
		sweep->this_command=NULL;
		sweep->cspod=0;
		sweep->ctruespod=0;
/*		sweep->buffer=MALLOC(4096); */
		sweep->bufpos=0;
		sweep->buffer[0]='\0';
		sweep->tmpbuf[0]='\0';
		sweep->name[0]='\0';
		sweep->next=NULL;
		for (count=0;count<10;count++) sweep->tellbuf[count]=NULL;

		swrite(NULL,sweep,"Flump is invite only, however if you want an account then just email the\nadmin at flump@amber.org.uk and we'll get back to you quickly, residency\nwill rely on an existing user recognising your spodname.\n\n");
		swrite(NULL,sweep,"If you have been invited by a current\nresident then just enter your alias (spodname) to setup an account.\n");
		swrite(NULL,sweep,config_get("prompt_login"));
	}
}

void close_connections()
{
	connection *sweep=NULL,*next=NULL;

	for(sweep=users; sweep; sweep=next)
	{
		next = sweep->next;
		swrite(NULL,sweep,"Socket Closing\n");
		close(sweep->fd);
		FREE(sweep);
	}
	close(main_descriptor);
}

void ping(connection *u)
{
	char temp[10];
	sprintf(temp,"%c%c%c",IAC,DO,TELOPT_STATUS);

	wsock(u->fd, temp);
}

void rping(connection *u)
{
	return;
}

short get_iac(connection *u)
{
	unsigned char iac_command[50];
	unsigned char in[2];
	short count,count2,width,height;
	char text[TMP_SIZE];

	if(!read(u->fd,in,1))
	{
		return 0;
	}
	iac_command[0]=in[0];
	if(iac_command[0]==255)
	{
		return 255;
	}
	if(iac_command[0]==DONT)
	{
		if(!read(u->fd,in,1))
		{
			return 0;
		}
		iac_command[1]=in[0];
		switch(iac_command[1])
		{
			default:
				sprintf(text,"%c%c%c",IAC,WONT,iac_command[1]);
				swrite(NULL,u,text);
		}
	}
	if(iac_command[0]==DO)
	{
		if(!read(u->fd,in,1))
		{
			return 0;
		}
		iac_command[1]=in[0];
		switch(iac_command[1])
		{
			case TELOPT_ECHO :
				break;
			default:
				sprintf(text,"%c%c%c",IAC,WONT,iac_command[1]);
				swrite(NULL,u,text);
		}
	}
	if(iac_command[0]==WILL)
	{
		if(!read(u->fd,in,1))
		{
			return 0;
		}
		iac_command[1]=in[0];
		switch(iac_command[1])
		{
			case TELOPT_TTYPE :
				sprintf(text,"%c%c%c%c%c%c",IAC,SB,TELOPT_TTYPE,1,IAC,SE);
				swrite(NULL,u,text);
				break;
			case TELOPT_NAWS :
				break;
			default:
				break;
		}
	}
	if(iac_command[0]==WONT)
	{
		if(!read(u->fd,in,1))
		{
			return 0;
		}
		iac_command[1]=in[0];
		switch(iac_command[1])
		{
			case TELOPT_STATUS:
				rping(u);
				break;
			default: break;
		}
	}
	if(iac_command[0]==SB)
	{
		count=2;
		while(iac_command[count-1]!=SE)
		{
			if(!read(u->fd,in,1))
			{
				return 0;
			}
			iac_command[count++]=in[0];
		}
		switch (iac_command[2])
		{
			case TELOPT_TTYPE :
				count2=0;
				for(count=4; iac_command[count]!=IAC; count++)
				{
					text[count2++]=iac_command[count];
				}
				text[count2]='\0';
				strcpy(u->ttype,text);
				break;
			case TELOPT_NAWS :
				width=(iac_command[3]*256)+iac_command[4];
				height=(iac_command[5]*256)+iac_command[6];
				u->twidth=width>20?width:21;
				u->theight=height>5?height:6;
				break;
			default:
				break;
		}
	}	
		
	return 0;
}

int input(connection *u)
{
	unsigned char tmp[2];

	if(!read(u->fd,tmp,1))
	{
		if (!u->quiet) raise_event(u,E_DISCONNECT,NULL,NULL);
		disconnect(u);
		return 0;
	}
	
	if (tmp[0]==IAC) {
		get_iac(u);
		return 0;
	}

	if ((tmp[0]==8 || tmp[0]==127) && u->bufpos>0) {
		u->bufpos--;
		return 0;
	}
	
/*	
	if (tmp[0]<32 || u->bufpos>=MAX_BUFFER-1) {
		u->last_command=time(0);
		strcpy(u->buffer,u->tmpbuf);
		u->tmpbuf[0]='\0';
		u->bufpos=0;
		debug(u->buffer);
		return 1;
	}
*/
	if (tmp[0]=='\n' || u->bufpos>=MAX_BUFFER-1) {
		if (u->quiet==0) raise_event(u,E_UNIDLE,NULL,NULL);
		u->last_command=time(0);
		strcpy(u->buffer,u->tmpbuf);
		u->tmpbuf[0]='\0';
		u->bufpos=0;
		debug(u->buffer);
		return 1;
	}

	if (tmp[0]<32) return 0;

	u->tmpbuf[u->bufpos]=tmp[0];
	u->bufpos++;
	u->tmpbuf[u->bufpos]='\0';
	return 0;
}

void disconnect(connection *u)
{
        connection *sweep=NULL;
	char *dat=NULL;
	time_t totalspod=0;
	char temp[TMP_SIZE];

	if (!u->quickdis) {
		dat=get_attribute(u->object,"totalspod");
		if (dat) {
			totalspod=atol(dat);
			if (totalspod<0) totalspod=0;
			FREE(dat);
		}	
		totalspod+=u->cspod;
		sprintf(temp,"%ld",(long)totalspod);
		set_attribute(u->object,"totalspod",temp);
		sprintf(temp,"%ld",(long)time(0));
		set_attribute(u->object,"laston",temp);
	}
	
        for(sweep=users; sweep; sweep=sweep->next)
        {
                if(sweep->next==u)
                {
                        sweep->next=u->next;
                }
        }
	if(users==u)
	{
		users=users->next;
	}
        close(u->fd);
        FREE(u);
}
