/* $Id: misc.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <flump.h>

char retval[TMP_SIZE];

int iscolour(char c)
{
	switch (c) {
		case 'r' :
		case 'g' :
		case 'y' :
		case 'b' :
		case 'p' :
		case 'm' :
		case 'c' :
		case 'w' :
		case 'H' :
		case 'N' :
		case 'R' :
		case 'G' :
		case 'Y' :
		case 'B' :
		case 'P' :
		case 'M' :
		case 'C' :
		case 'A' :
		case 'W' : return 1; break;
		default  : return 0; break;
	}

	return 0;
}

char *daysago(time_t num)
{
	short days=0;

	days=(int)(num/86400);

	switch (days) {
		case 0: sprintf(retval,"today"); break;
		case 1: sprintf(retval,"yesterday"); break;
		default: sprintf(retval, "%d days ago",days); break;
	}

	return retval;
}

char *timeval(time_t num)
{
	short years=0,weeks=0,days=0,hrs=0,mins=0,secs=0;
	short start=0;
	char tmp[255];

	years=(int)(num/31449600);

	if (years) {
		weeks=(int)((num%31449600)/604800);
	}
	else
	{
		weeks=(int)(num/604800);
	}

	if (weeks||years) {
		days=(int)((num%604800)/86400);
	}
	else
	{
		days=(int)(num/86400);
		hrs=(int)((num%86400)/3600);
		mins=(int)((num%3600)/60);
		secs=(int)(num%60);
	}

	retval[0]='\0';
	tmp[0]='\0';
	if (weeks||years) {
		if (years) {
			sprintf(retval,"%dyear%s %dweek%s %dday%s",years,years==1?"":"s",weeks,weeks==1?"":"s",days,days==1?"":"s");
		}
		else
		{
			sprintf(retval,"%dweek%s %dday%s",weeks,weeks==1?"":"s",days,days==1?"":"s");
		}
	}
	else
	{
		if (days) {
			sprintf(tmp,"%dday%s",days,days==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}

		if (hrs||start) {
			sprintf(tmp,"%s%dhr%s",start?" ":"",hrs,hrs==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}

		if (mins||start) {
			sprintf(tmp,"%s%dmin%s",start?" ":"",mins,mins==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}

		if (secs||start) {
			sprintf(tmp,"%s%dsec%s",start?" ":"",secs,secs==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}
	}
		
	return retval;
}

void stolower(char *str)
{
        char *p=NULL;

        for (p=str;*p!='\0';p++)
        {
                *p=tolower(*p);
        }
}

/** look at this for possible memory leak
*/
char *ele_delim(char *buf, int index)
{
	char *moo=NULL;
	char *ele=NULL;
	int count=0;

	for (ele=strtok_r(buf,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		if (count==index)
			return ele;
		count++;	
	}

	return NULL;
}

/* okay using a copy of buf1 because strtok is a useless piece of shit
*/
int in_delim(char *buf1, char *what)
{
	char *moo=NULL;
	char *ele=NULL;
	char *buf=NULL;

	if (!buf1) return 0;
	if (!what) return 0;

	buf=MALLOC(strlen(buf1)+4,"in_delim:buf");
	strcpy(buf,buf1);

	for (ele=strtok_r(buf,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		if (strcasecmp(ele,what)==0)
		{	
			FREE(buf);
			return 1;
		}	
	}

	FREE(buf);
	return 0;
}

void insert_into(char **buf, char *what)
{
	char *moo=NULL;
	char *ele=NULL;
	char *buf2=NULL;

	buf2=MALLOC((strlen(*buf)+2)*sizeof(char),"insert_into:buf2");
	strcpy(buf2,*buf);

	for (ele=strtok_r(*buf,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
	{
		if (strcasecmp(ele,what)==0) 
		{
			FREE(buf2);
			return;
		}	
	}

	strcpy(*buf,buf2);
	FREE(buf2);
	/* needs adding */

	*buf=realloc(*buf,(strlen(*buf)+strlen(what)+4)*sizeof(char));
	if (*buf[0]!='\0') strcat(*buf,",");
	strcat(*buf,what);
}

void remove_from(char **buf, char *what)
{
	char *buf2=NULL;
	char *moo=NULL;
	char *ele=NULL;

	if (*buf) {
		buf2=MALLOC((strlen(*buf)+2)*sizeof(char),"remove_from:buf2");

		strcpy(buf2,*buf);

		*buf[0]='\0';

		for (ele=strtok_r(buf2,",",&moo); ele; ele=strtok_r(NULL,",",&moo))
		{
			if (strcmp(ele,what)!=0) {
				if (*buf[0]!='\0') {
					strcat(*buf,",");
				}
				strcat(*buf,ele);
			}
		}
		*buf=realloc(*buf,(strlen(*buf)+2)*sizeof(char));
		FREE(buf2);
	}
}

/*** Tell telnet not to echo characters - for password entry ***/
void echo_off(connection *u)
{
char seq[4];

sprintf(seq,"%c%c%c",IAC,WILL,TELOPT_ECHO);
swrite(NULL,u,seq);
}


/*** Tell telnet to echo characters ***/
void echo_on(connection *u)
{
char seq[4];

sprintf(seq,"%c%c%c",IAC,WONT,TELOPT_ECHO);
swrite(NULL,u,seq);
}

void doprompt(connection *c, char *str)
{
	unsigned char *tmpstr=NULL;
	tmpstr=MALLOC(4096,"doprompt:tmpstr");
	sprintf(tmpstr,"%s%c%c",str,IAC,GA);
	swrite(NULL,c,tmpstr);
	/* wsock(c->fd, tmpstr); */
	FREE(tmpstr);
}

char *getgen(connection *u, char *m, char *f, char *n)
{
	char *gen=NULL;
	char *ret=NULL;

	gen=get_attribute(u->object,"gender");

	if (!gen) return n;

	switch (gen[0]) {
		case 'M': ret=m; break;
		case 'F': ret=f; break;
		case 'N': ret=n; break;
		default: ret=n; break;
	}	

	FREE(gen);
	return ret;
}

/* if we can find a prefer module function then use it, otherwise return NULL
*/
connection *use_prefer(connection *u, char *name)
{
	connection *(*func)(connection *u, char *name);

	if (!name) return NULL;

	func=dlsym(RTLD_DEFAULT, "get_prefer");

	if (!func) return NULL;

	return func(u, name);
}

/* get a users connection from the name
** now takes into account prefer, if lists are loaded
*/
connection *getubyname(connection *u, char *name, char *errormsg)
{
	connection *sweep=NULL;
	connection *found=NULL;
	int spit=0;

	if (!name) return NULL;
	if (u) {
		found=use_prefer(u, name);
		if (found) return found;
		found=NULL;
	}

	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		if (strncasecmp(sweep->name,name,strlen(name))==0) {
			if (found) {
				if (!spit) {
					sprintf(errormsg,"Multiple matches: %s",found->name);
					spit=1;
				}	
				strcat(errormsg,", ");
				strcat(errormsg,sweep->name);
			}
			found=sweep;
		}
	}

	if (!found) strcpy(errormsg,"No such user online.\n");
	if (spit) {
		strcat(errormsg,".\n");
		found=NULL;
	}	
	return found;
}	

/* removes spaces from the start and end of a string
** this will cause no end of problems if you pass a NULL
** it also may fuck up if you pass a zero length string.
**
** the returned value shouldn't be FREE'd it will be a subset of the
** original string.
*/
char *stripspace(char *str)
{
	char *search=NULL;
	char *search2=NULL;

	if (!str) return NULL;
	/* strip spaces at the start */
	for (search=str;*search==' '&&*search!='\0';search++);

	if (*search=='\0') return search;

	for (search2=strchr(str,'\0');*search2=='\0'||*search2==' ';search2--) {
		*search2='\0';
	}	

	return search;
}

