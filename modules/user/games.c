#include "flump.h"

#define HEADS 1
#define TAILS 2

int psuedo_rand(int max)
{
        return (1+(int) ((double)max*rand()/(RAND_MAX+1.0)));
}

static void ht(connection *u, int which, char *arg)
{
	short win=0;
	short num=0;
	short gam=0;
	long cash=0;
	char *muff=NULL;
	char temp[4096];

	if (!arg) {
		swrite(NULL,u,"[coin] please specify an amount you wish to gamble\n");
		return;
	}

	gam=atoi(arg);

	if (gam<1 || gam>1000) {
		swrite(NULL,u,"[coin] you must gamble between 1 and 1000 flimps\n");
		return;
	}

	num=psuedo_rand(2);

	if (num==2 && which==HEADS) {
		win=1;
	}
	if (num==1 && which==TAILS) {
		win=1;
	}

	if (win) {
		sprintf(temp, "[coin] ^Y%s^N, you win!!\n", which==HEADS?"Heads":"Tails");
	}
	else
	{
		sprintf(temp, "[coin] ^Y%s^N, you lose\n", which==HEADS?"Tails":"Heads");
		gam-=(gam*2);
	}
	swrite(NULL,u,temp);

	muff=get_attribute(u->object,"money");
	if (muff) {
		cash=atol(muff);
		FREE(muff);
	}	
	cash+=gam;
	sprintf(temp,"%ld",cash);
	set_attribute(u->object,"money",temp);
}

void heads(connection *u, char *arg)
{
	ht(u,HEADS,arg);
}

void tails(connection *u, char *arg)
{
	ht(u,TAILS,arg);
}

int init()
{
	add_command("heads",&heads,NULL,NULL);
	add_command("tails",&tails,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("heads",NULL);
	remove_command("tails",NULL);
	return 1;
}

char *version()
{
	return "Quick games (c) 2005 Karl Bastiman $Revision: 2.2 $";
}
