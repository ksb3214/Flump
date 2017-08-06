#include "flump.h"

#define INTERVAL	60

static time_t lastweb=0;

void tick()
{
	connection *sweep=NULL;
	time_t timenow;
	char temp[16353];
	FILE *fp=NULL;
	int count=0;

	timenow=time(0);

	if (timenow-lastweb<(INTERVAL)) return;
	lastweb=timenow;
	sprintf(temp,"Users on now<br><br>");

	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		count++;
		strcat(temp, sweep->name);
		strcat(temp, "<br>");
	}

	if (!(fp=fopen("/home/flump/public_html/flumpweb/userson.php","w"))) {
		return;
	}
	fprintf(fp,temp);
	fclose(fp);
}

char *version()
{
	return "Webstats (c) 2005 Karl Bastiman $Revision: 2.7 $";
}

int init()
{
	lastweb=0;
	return 1;
}

int uninit()
{
	return 1;
}

