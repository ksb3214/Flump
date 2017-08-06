#include "flump.h"

time_t lastgift=0;

int psuedo_rand(int max)
{
	return (1+(int) ((double)max*rand()/(RAND_MAX+1.0)));
}

void richlist(connection *u, char *arg)
{
	struct lists_st *users;
	struct lists_st *sweep;
	char tmp[TMP_SIZE];
	int count=0;

	users = getvalues(OB_USER, "money");
	if(!users)
	{
		return;
	}

	for(sweep=users; sweep; sweep=sweep->next) count++;

	if(count>0)
	{
		unsigned long oid[count],toid;
		long value[count],tvalue;
		int x,y;

		x=0;
		for(sweep=users; sweep; sweep=sweep->next)
		{
			oid[x]=sweep->oid;
			value[x]=atol(sweep->value);
			x++;
		}

		for(y=0; y<count-1; y++)
		{
			for(x=0; x<count-1; x++)
			{
				if(value[x]<value[x+1])
				{
					tvalue=value[x];
					value[x]=value[x+1];
					value[x+1]=tvalue;
					toid=oid[x];
					oid[x]=oid[x+1];
					oid[x+1]=toid;
				}
			}
		}
		format_bar(u,"Rich list",tmp);
		swrite(NULL,u,tmp);
		for(x=0; x<count && x<10; x++)
		{
			sprintf(tmp,"      ^C%8ld ^N:^Y %s^N\n",value[x],db_get_obj_name(oid[x]));
			swrite(NULL,u,tmp);
		}
		format_bar(u,NULL,tmp);
		swrite(NULL,u,tmp);
	}
	freelist(users);
}

void tick()
{
	connection *sweep=NULL;
	long cash=0;
	char *muff=NULL;
	time_t timenow;
	char temp[4096];
	int count=0;
	int whom=0;
	int gift=0;

	timenow=time(0);

	if (timenow-lastgift<(7200)) return;
	lastgift=timenow;

	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		count++;
	}

	if (count<=2) return;


	whom=psuedo_rand(count);

	printf("Count %d, whom %d\n",count,whom);
	count=0;
	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		count++;
		if (count==whom) {
			cash=0;
			muff=get_attribute(sweep->object,"money");
			if (muff) {
				cash=atol(muff);
				FREE(muff);
			}
			gift=psuedo_rand(190)+10;
			printf("cash %ld whom %d gift %d\n", cash,whom,gift);
			cash+=gift;
			sprintf(temp,"%ld",cash);
			set_attribute(sweep->object,"money",temp);
			sprintf(temp, "[bank] For being such a nice spod, you recieve %d flimps.\n", gift);
			swrite(NULL,sweep,temp);
		}
	}

}

char *version()
{
	return "Flimps (c) 2005 Karl Bastiman $Revision: 2.7 $";
}

int init()
{
	add_command("richlist",&richlist,NULL,NULL);
	lastgift=0;
	return 1;
}

int uninit()
{
	remove_command("richlist",NULL);
	return 1;
}

