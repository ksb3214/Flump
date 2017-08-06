/* $Id: bling.c,v 2.14 2005/07/28 10:46:37 ksb  Exp $ */
/* $Revision: 2.14 $ */

/*******************************************************
 * bling.c - Stuff to spend your flimps on             *
 *******************************************************/

#include <string.h>
#include "flump.h"

#define L_LONG 0 
#define L_SHORT 1
#define L_DESC 2

extern char *ele_delim(char *, int);

/* struct data_st *xinfo; */

unsigned long myobject=0;

long price_of(char *item)
{
	char *price = NULL;
	long p = 0;
	char tmp[TMP_SIZE];


	sprintf(tmp,"%s_price",item);

	price = get_attribute(myobject,tmp);
	if(!price)
	{
		return 0;
	}

	p = atol(price);
	free(price);
	return(p);
}

int player_has(connection *u, char *item)
{
	char *posessions = NULL;
	int has = 0;

	posessions=get_attribute(u->object,"bling");
	if(!posessions)
	{
		return has;
	}

	if(in_delim(posessions,item))
	{
		has=1;
	}

	free(posessions);
	return has;
}

void bling_list_item(connection *u, char *item, int style)
{
	char tmp[TMP_SIZE];
	char *name = NULL;
	char *desc = NULL;
	char *price = NULL;

	sprintf(tmp,"%s_name",item);
	name = get_attribute(myobject,tmp);
	if(!name) return;
	sprintf(tmp,"%s_desc",item);
	desc = get_attribute(myobject,tmp);
	if(!desc) 
	{
		free(name);
		return;
	}
	sprintf(tmp,"%s_price",item);
	price = get_attribute(myobject,tmp);
	if(!price) 
	{
		free(name);
		free(desc);
		return;
	}

	switch(style)
	{
		case L_LONG : sprintf(tmp,"^C%-10s^N | ^Y%-30s^N                    ^C%10s^N\n           | ^y%-60s^N\n",item,name,price,desc);
			break;
		case L_DESC : sprintf(tmp,"^Y%s ^C(%s)^N\n",desc,item);
			break;
		case L_SHORT : sprintf(tmp,"^Y%s^N\n",name);
			break;
		default : sprintf(tmp,"Some moron can't code!\n");
			break;
	}

	swrite(NULL,u,tmp);
	free(name);
	free(desc);
	free(price);
}

/******************************************************
 * List items of bling                                *
 ******************************************************/
void bling_list(connection *u,char *arg)
{
	char *objectlist = NULL;
	char *item = NULL;
	char *pos = NULL;
	char *money_c = NULL;
	long money = 0;
	char tmp[TMP_SIZE];

	objectlist = get_attribute(myobject,"items");
	if(!objectlist || objectlist==0)
	{
		swrite(NULL,u,"Unfortunately, there are no bling objects yet.\n");
		return;
	}

	money_c = get_attribute(u->object,"money");
	if(!money_c)
	{
		swrite(NULL,u,"You have no money.\n");
		free(objectlist);
		return;
	}

	money = atol(money_c);
	free(money_c);

	format_bar(u,"Items available",tmp);
	swrite(NULL,u,tmp);

	for(item = strtok_r(objectlist,",",&pos); item; item = strtok_r(NULL,",",&pos))
	{
		if(player_has(u,item)==0)
		{
			if(money>=price_of(item))
				bling_list_item(u,item,L_LONG);
		}
	}
	format_bar(u,NULL,tmp);
	swrite(NULL,u,tmp);
			
	free(objectlist);
}

/******************************************************
 * List ALL items of bling                            *
 ******************************************************/
void bling_list_all(connection *u,char *arg)
{
	char *objectlist = NULL;
	char *item = NULL;
	char *pos = NULL;
	char tmp[TMP_SIZE];

	objectlist = get_attribute(myobject,"items");
	if(!objectlist || objectlist==0)
	{
		swrite(NULL,u,"Unfortunately, there are no bling objects yet.\n");
		return;
	}

	format_bar(u,"Items available",tmp);
	swrite(NULL,u,tmp);

	for(item = strtok_r(objectlist,",",&pos); item; item = strtok_r(NULL,",",&pos))
	{
		bling_list_item(u,item,L_LONG);
	}
	format_bar(u,NULL,tmp);
	swrite(NULL,u,tmp);

	free(objectlist);
}


/******************************************************
 * Buy a bling item if you don't already have it      *
 ******************************************************/
void bling_buy(connection *u, char *item)
{
	char *objectlist = NULL;
	char *posessions = NULL;
	char tmp[TMP_SIZE];
	char *money_c = NULL;
	long money = 0;

	objectlist = get_attribute(myobject,"items");
	if(!objectlist || objectlist==0)
	{
		swrite(NULL,u,"Unfortunately, there are no bling objects yet.\n");
		return;
	}

	if(!in_delim(objectlist,item))
	{
		sprintf(tmp,"There is no such thing as a %s.\n",item);
		swrite(NULL,u,tmp);
		free(objectlist);
		return;
	}

	posessions = get_attribute(u->object,"bling");
	if(posessions)
	{
		if(in_delim(posessions,item))
		{
			sprintf(tmp,"You already have a %s.\n",item);
			swrite(NULL,u,tmp);
			free(objectlist);
			free(posessions);
			return;
		}
	}

	if(!posessions)
	{
		posessions=malloc(1);
		bzero(posessions,1);
	}

	money_c = get_attribute(u->object,"money");
	if(money_c)
	{
		money = atol(money_c);
		free(money_c);
	} else {
		money=0;
	}

	if(money<price_of(item))
	{
		sprintf(tmp,"You cannot afford a %s.\n",item);
		swrite(NULL,u,tmp);
		free(posessions);
		free(objectlist);
		return;
	}

	money-=price_of(item);
	sprintf(tmp,"%ld",money);
	set_attribute(u->object,"money",tmp);
	insert_into(&posessions,item); 
	set_attribute(u->object,"bling",posessions);
	sprintf(tmp,"You buy a %s.\n",item);
	swrite(NULL,u,tmp);
	free(objectlist);
	free(posessions);
}

/******************************************************
 * Sell one of your bling posessions                  *
 ******************************************************/
void bling_sell(connection *u, char *item)
{
	char *posessions = NULL;
	char *money_c = NULL;
	long money = 0;
	char tmp[TMP_SIZE];

	posessions = get_attribute(u->object,"bling");
	if(!posessions)
	{
		swrite(NULL,u,"You have nothing to sell.\n");
		return;
	}

	if(in_delim(posessions,item))
	{
		money_c = get_attribute(u->object,"money");
		if(!money_c)
		{
			money=0;
		} else {
			money = atol(money_c);
			free(money_c);
		}

		money+=price_of(item);
		sprintf(tmp,"%ld",money);
		set_attribute(u->object,"money",tmp);
		remove_from(&posessions,item);
		set_attribute(u->object,"bling",posessions);
		free(posessions);
		sprintf(tmp,"You sell your %s.\n",item);
		swrite(NULL,u,tmp);
		return;
	} 
	sprintf(tmp,"You have no %s to sell.\n",item);
	swrite(NULL,u,tmp);
	free(posessions);
	return;
}

/******************************************************
 * List your bling posessions                         *
 ******************************************************/
void bling_show(connection *u, char *arg)
{
	char *posessions = NULL;
	char *pos = NULL;
	char *item = NULL;
	long total = 0;
	char tmp[TMP_SIZE];
	char tmp2[TMP_SIZE];
	char who[TMP_SIZE];
	unsigned long *oid = NULL;

	if(arg)
	{
		oid=db_get_objid(arg,OB_USER);
		if(!oid)
		{
			swrite(NULL,u,"No such user.\n");
			return;
		}
		sprintf(who,"%s",db_get_obj_name(*oid));
	} else {
		oid = &u->object;
		sprintf(who,"you");
	}

	posessions = get_attribute(*oid,"bling");

	if(!posessions || posessions==0)
	{
		if(arg)
		{
			sprintf(tmp,"%s has no posessions.\n",who);
			swrite(NULL,u,tmp);
		} else {
			swrite(NULL,u,"You have no posessions.\n");
		}
		return;
	}
	
	sprintf(tmp2,"Items owned by %s",who);
	format_bar(u,tmp2,tmp);
	swrite(NULL,u,tmp);

	for(item = strtok_r(posessions,",",&pos); item; item = strtok_r(NULL,",",&pos))
	{
		total+=price_of(item);
		bling_list_item(u,item,L_DESC);
	}
	free(posessions);
	sprintf(tmp2,"Posessions are valued at %lu flimps",total);
	format_bar(u,tmp2,tmp);
	swrite(NULL,u,tmp);
}

/******************************************************
 * Return a user's bling value                        *
 ******************************************************/
long bling_value(unsigned long oid)
{
	char *posessions = NULL;
	char *item = NULL;
	char *pos = NULL;
	long total = 0;

	posessions = get_attribute(oid,"bling");
	if(!posessions)
	{
		return(0);
	}
	for(item = strtok_r(posessions,",",&pos); item; item = strtok_r(NULL,",",&pos))
	{
		total+=price_of(item);
	}
	free(posessions);
	return(total);
}
	
/******************************************************
 * Create an item of bling                            *
 ******************************************************/
void bling_get_price(connection *u, char *arg)
{
	char tmp[TMP_SIZE];
	char tmp2[TMP_SIZE];
	char *item = NULL;
	long price = 0;

	if(!arg)
	{
		swrite(NULL,u,"Enter price:\n");
		u->function=&bling_get_price;
		return;
	}
	
	item = get_attribute(u->object,"bling_edit");
	if(!item)
	{
		u->function=&run_command;
		swrite(NULL,u,"Error: I cannot find your edit object.  Aborting.\n");
		showprompt(u);
		return;
	}
	price=atol(arg);
	if(price<=0)
	{
		free(item);
		swrite(NULL,u,"Enter price:\n");
		u->function=&bling_get_price;
		return;
	}

	sprintf(tmp,"%s_price",item);
	sprintf(tmp2,"%lu",price);
	set_attribute(myobject,tmp,tmp2);
	free(item);
	delete_attribute(u->object,"bling_edit");
	u->function=&run_command;
	swrite(NULL,u,"Item created.\n");
	showprompt(u);
	return;
}

void bling_get_description(connection *u, char *arg)
{
	char tmp[TMP_SIZE];
	char *item = NULL;

	if(!arg)
	{
		swrite(NULL,u,"Enter description:\n");
		u->function=&bling_get_description;
		return;
	}
	
	if(strlen(arg)==0)
	{
		swrite(NULL,u,"Enter description:\n");
		u->function=&bling_get_description;
		return;
	}
	
	if(strlen(arg)>60)
	{
		swrite(NULL,u,"Description too long - max 60 chars\n");
		swrite(NULL,u,"Enter description:\n");
		u->function=&bling_get_description;
		return;
	}
	
	item = get_attribute(u->object,"bling_edit");
	if(!item)
	{
		u->function=&run_command;
		swrite(NULL,u,"Error: I cannot find your edit object.  Aborting.\n");
		showprompt(u);
		return;
	}
	sprintf(tmp,"%s_desc",item);
	set_attribute(myobject,tmp,arg);
	free(item);
	swrite(NULL,u,"Enter price:\n");
	u->function=&bling_get_price;
	return;
}

void bling_get_short_name(connection *u, char *arg)
{
	char tmp[TMP_SIZE];
	char *item = NULL;

	if(!arg)
	{
		swrite(NULL,u,"Enter short name:\n");
		u->function=&bling_get_short_name;
		return;
	}

	if(strlen(arg)==0)
	{
		swrite(NULL,u,"Enter short name:\n");
		u->function=&bling_get_short_name;
		return;
	}

	if(strlen(arg)>30)
	{
		swrite(NULL,u,"Short name too long - max 30 chars\n");
		swrite(NULL,u,"Enter short name:\n");
		u->function=&bling_get_short_name;
		return;
	}

	item = get_attribute(u->object,"bling_edit");
	if(!item)
	{
		u->function=&run_command;
		swrite(NULL,u,"Error: I cannot find your edit object.  Aborting.\n");
		showprompt(u);
		return;
	}
	sprintf(tmp,"%s_name",item);
	set_attribute(myobject,tmp,arg);
	free(item);
	swrite(NULL,u,"Enter description:\n");
	u->function=&bling_get_description;
	return;
}
	

void bling_create(connection *u, char *arg)
{
	int bad = 0;
	char *sweep = NULL;
	char *objectlist = NULL;

	if(!arg)
	{
		swrite(NULL,u,"Usage: blingcreate <item name>\n");
		return;
	}

	bad=strlen(arg);
	for(sweep=arg; *sweep; sweep++)
	{
		if((*sweep>='a' && *sweep<='z') || (*sweep>='0' && *sweep<='9'))
		{
			bad--;
		}
	}
	if(bad>0)
	{
		swrite(NULL,u,"Error: Bad characters in item name.\n");
		return;
	}

	if(strlen(arg)>10)
	{
		swrite(NULL,u,"Item name too long - 10 chars max\n");
		return;
	}
	objectlist = get_attribute(myobject,"items");
	{
		if(!objectlist)
		{
			objectlist = malloc(1);
			bzero(objectlist,1);
		}
	}
	if(in_delim(objectlist,arg))
	{
		swrite(NULL,u,"Error: That item already exists.\n");
		free(objectlist);
		return;
	}
	insert_into(&objectlist,arg);
	set_attribute(myobject,"items",objectlist);
	free(objectlist);
	set_attribute(u->object,"bling_edit",arg);
	swrite(NULL,u,"Enter short name:\n");
	u->function=&bling_get_short_name;
}

void bling_top(connection *u, char *arg)
{
	struct lists_st *allusers = NULL;
	struct lists_st *sweep = NULL;
	char tmp[TMP_SIZE];
	int count=0;
	char *item = NULL;
	char *pos= NULL;
	int x=0;
	int y=0;
	int total=0;

	allusers = getvalues(OB_USER, "bling");
	if(!allusers)
	{
		return;
	}
	for(sweep=allusers; sweep; sweep=sweep->next)
	{
		count++;
	}
	if(count>0)
	{
		unsigned long oid[count],toid=0;
		long value[count],tvalue=0;

		x=0;
		for(sweep=allusers; sweep; sweep=sweep->next)
		{
			total=0;
			oid[x]=sweep->oid;
			for(item = strtok_r(sweep->value,",",&pos); item; item = strtok_r(NULL,",",&pos))
			{
				total+=price_of(item);
			}
			value[x]=total;
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
		format_bar(u,"Top ten pimps",tmp);
		swrite(NULL,u,tmp);
		for(x=0; x<count && x<10; x++)
		{
			sprintf(tmp,"      ^C%8lu ^N:^Y %s^N\n",value[x],db_get_obj_name(oid[x]));
			swrite(NULL,u,tmp);
		}
		format_bar(u,NULL,tmp);
		swrite(NULL,u,tmp);
	}
	freelist(allusers);
}

void bling_usage(connection *u)
{
	swrite(NULL,u,"Usage: bling list\n");
	swrite(NULL,u,"       bling show [<user>]\n");
	swrite(NULL,u,"       bling buy <item>\n");
	swrite(NULL,u,"       bling sell <item>\n");
	swrite(NULL,u,"       bling top\n");
}

/******************************************************
 * Main bling dispacher                               *
 ******************************************************/
void bling(connection *u,char *arg)
{
	char param[TMP_SIZE];
	if(!arg)
	{
		bling_usage(u);
		return;
	}
	if(!strcmp(arg,"list"))
	{
		bling_list(u,NULL);
		return;
	}
	if(sscanf(arg,"buy %s",param)==1)
	{
		bling_buy(u,param);
		return;
	}
	if(!strcmp(arg,"show"))
	{
		bling_show(u,NULL);
		return;
	}
	if(sscanf(arg,"show %s",param)==1)
	{
		bling_show(u,param);
		return;
	}
	if(sscanf(arg,"sell %s",param))
	{
		bling_sell(u,param);
		return;
	}
	if(!strcmp(arg,"top"))
	{
		bling_top(u,NULL);
		return;
	}
	bling_usage(u);
}

/******************************************************
 * Refund a bling item                                *
 ******************************************************/
void refund(connection *u,char *item)
{
	struct lists_st *users = NULL;
	struct lists_st *sweep = NULL;
	char *money_c = NULL;
	long money = 0;
	char tmp[TMP_SIZE];

	users = getvalues(OB_USER, "bling");
	if(!users)
	{
		return;
	}
	for(sweep=users; sweep; sweep=sweep->next)
	{
		if(in_delim(sweep->value,item))
		{
			remove_from(&sweep->value,item);
			set_attribute(sweep->oid,"bling",sweep->value);
			money_c = get_attribute(sweep->oid,"money");
			if(!money_c)
			{
				money=0;
			} else {
				money=atol(money_c);
				free(money_c);
			}
			money+=price_of(item);
			sprintf(tmp,"%ld",money);
			set_attribute(sweep->oid,"money",tmp);
		}
	}
	freelist(users);
}

/******************************************************
 * Delete a bling item                                *
 ******************************************************/
void bling_delete(connection *u, char *arg)
{
	char *objectlist = NULL;
	char tmp[TMP_SIZE];

	if(!arg)
	{
		swrite(NULL,u,"Usage: blingdelete <item name>\n");
		return;
	}

	objectlist = get_attribute(myobject,"items");
	if(!objectlist)
	{
		swrite(NULL,u,"Item does not exist.\n");
		return;
	}
	if(!in_delim(objectlist,arg))
	{
		swrite(NULL,u,"Item does not exist.\n");
		return;
	}
	
	refund(u,arg);
	remove_from(&objectlist,arg);
	set_attribute(myobject,"items",objectlist);
	free(objectlist);
	sprintf(tmp,"%s_name",arg);
	delete_attribute(myobject,tmp);
	sprintf(tmp,"%s_desc",arg);
	delete_attribute(myobject,tmp);
	sprintf(tmp,"%s_price",arg);
	delete_attribute(myobject,tmp);
	swrite(NULL,u,"Item deleted.\n");
}

void examine_info(connection *u, unsigned long target)
{
	long v;
	char tmp[TMP_SIZE];

	v = bling_value(target);
	sprintf(tmp,"^GBling value^N: %lu flimps\n",v);
	swrite(NULL,u,tmp);
}

void bankrupt(connection *u, char *arg)
{
	unsigned long *target = NULL;
	char *moneyc = NULL;
	char *bankruptc = NULL;
	long money = 0;
	long bankrupt = 0;
	char temp[TMP_SIZE];

	if(!arg)
	{
		swrite(NULL,u,"Usage: bankrupt <player>\n");
		return;
	}
	target=db_get_objid(arg,OB_USER);
	if(!target)
	{
		swrite(NULL,u,"Cannot locate that player.\n");
		return;
	}
	moneyc = get_attribute(*target,"money");
	if(!moneyc)
	{
		swrite(NULL,u,"That person is not in enough debt.\n");
		return;
	}
	money = atol(moneyc);
	free(moneyc);
	if(money>-10000)
	{
		swrite(NULL,u,"That person is not in enough debt.\n");
		return;
	}
	set_attribute(*target,"money","0");
	set_attribute(*target,"bling","");
	bankruptc = get_attribute(*target,"bankrupt");
	if(!bankruptc)
	{
		bankrupt=0;
	} else {
		bankrupt=atol(bankruptc);
		free(bankruptc);
	}
	bankrupt++;
	sprintf(temp,"%ld",bankrupt);
	set_attribute(*target,"bankrupt",temp);
	swrite(NULL,u,"Player bankrupted.\n");
}

int init()
{
	myobject = register_module("bling");
	
	if(myobject==0)
	{
		return 0;
	}

	add_command("bling",&bling,NULL,NULL);
	add_command("blingcreate",&bling_create,NULL,NULL);
	add_command("blingdelete",&bling_delete,NULL,NULL);
	add_command("blinglist",&bling_list_all,NULL,NULL);
	add_command("bankrupt",&bankrupt,NULL,NULL);
/*	xinfo = register_data("examine",&examine_info,DATA_PUBLIC); */
	return 1;
}

int uninit()
{
	remove_command("bling",NULL);
	remove_command("blingcreate",NULL);
	remove_command("blingdelete",NULL);
	remove_command("blinglist",NULL);
	remove_command("bankrupt",NULL);
/*	unregister_data(xinfo); */
	return 1;
}

char *version()
{
	return "Bling system (c) 2005 Karl Bastiman $Revision: 2.14 $";
}
