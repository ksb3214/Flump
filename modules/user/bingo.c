#include "flump.h"
#include "bingo.h"

#define GAMEWAIT	5 /* minutes */
#define CALLWAIT	10 /* seconds */
#define LASTWAIT	30 /* seconds */

struct card_st {
	char name[128];
	unsigned char card[27]; /* 3x9 */
	char game;
	short baddaub;
	struct card_st *next;
};

/* calling numbers, as each is called an x is placed */
char numbers[90];
struct card_st *cards=NULL;
time_t nextgame=0;
time_t lastcall=0;
short current=0;
short warned=0;
long prize=0;
long minbet=0;

struct card_st *getcard(connection *u);

int psuedo_rand(int max)
{
        return (1+(int) ((double)max*rand()/(RAND_MAX+1.0)));
}

/* does not obey ignore */
void sbingo(connection *u, char *txt)
{
	connection *sweep=NULL;
	char *muff=NULL;

	for(sweep=users; sweep; sweep=sweep->next)
	{
		if (sweep==u) continue;
		if (sweep->quiet) continue;
		if (!getcard(sweep)) {
			muff=get_attribute(sweep->object,"muffle_bingo");
			if (muff) {
				if (muff[0]=='Y') {
					FREE(muff);
					continue;
				}
				FREE(muff);
			}	
		}
		swrite(NULL,sweep,txt);
	}
}

/* does not obey ignore */
/* message only goes to those with cards */
void pbingo(connection *u, char *txt)
{
	connection *sweep=NULL;

	for(sweep=users; sweep; sweep=sweep->next)
	{
		if (sweep==u) continue;
		if (sweep->quiet) continue;
		if (!getcard(sweep)) continue;
		swrite(NULL,sweep,txt);
	}
}

void init_game()
{
	short count=0;
	struct card_st *sweep=NULL;
	struct card_st *next=NULL;

	for (count=0;count<90;count++) numbers[count]=0;

	/* remove all cards which are game=0 */

	for (sweep=cards;sweep;sweep=next) {
		next=sweep->next;
		sweep->game=0;
	}
}

void remove_current()
{
	struct card_st *sweep=NULL;
	struct card_st *next=NULL;
	struct card_st *last=NULL;
			
	/* remove all cards which are game=0 */

	for (sweep=cards;sweep;sweep=next) {
		next=sweep->next;
		if (sweep->game==0) {
			if (sweep==cards) {
				cards=sweep->next;
				free(sweep);
			}
			else
			{
				last->next=sweep->next;
				free(sweep);
			}
		}
		else
		{
			last=sweep;
		}
	}
}

short numleft()
{
	short count=0;
	short total=0;

	for (count=0;count<90;count++) {
		if (numbers[count]!='X') total++;
	}

	return total;
}

void create_card(char *name)
{
	char *card=NULL;
	struct card_st *sweep=NULL;
	short count=0;
	short gotnum=0;
	short pos=0;
	short row=0;
	short col=0;
	unsigned char num=0;
	short dupe=0;

	if (!cards) {
		cards=(struct card_st *)malloc(sizeof(struct card_st));
		strcpy(cards->name,name);
		for (count=0;count<27;count++) {
			cards->card[count]=99;
		}
		cards->next=NULL;
		cards->game=1;
		cards->baddaub=0;
		card=cards->card;
	}
	else
	{
		for (sweep=cards;sweep->next;sweep=sweep->next);

		sweep->next=(struct card_st *)malloc(sizeof(struct card_st));
		sweep=sweep->next;
		strcpy(sweep->name,name);
		for (count=0;count<27;count++) {
			sweep->card[count]=99;
		}
		sweep->next=NULL;
		sweep->game=1;
		sweep->baddaub=0;
		card=sweep->card;
	}

	for (row=0;row<3;row++) {
		gotnum=0;
		while (gotnum<5) {
			num=psuedo_rand(90);
			dupe=0;
			for (count=0;count<27;count++) {
				if (card[count]==num) dupe=1;
			}
			if (dupe) continue;

			switch (num) {
				case 1 ... 10 : col=1; break;
				case 11 ... 20 : col=2; break;
				case 21 ... 30 : col=3; break;
				case 31 ... 40 : col=4; break;
				case 41 ... 50 : col=5; break;
				case 51 ... 60 : col=6; break;
				case 61 ... 70 : col=7; break;
				case 71 ... 80 : col=8; break;
				case 81 ... 90 : col=9; break;
				default : col=0; break;
			}
			if (col==0) continue;
			pos=(col-1)+(row*9);
			if (card[pos]!=99) continue;
			card[pos]=num;
			gotnum++;
		}
	}

	/* order the card */
	for (col=0;col<9;col++) {
		if (card[col+18]!=99&&card[col]!=99) {
			if (card[col+18]<card[col]) {
				num=card[col];
				card[col]=card[col+18];
				card[col+18]=num;
			}
		}
		if (card[col+18]!=99&&card[col+9]!=99) {
			if (card[col+18]<card[col+9]) {
				num=card[col+9];
				card[col+9]=card[col+18];
				card[col+18]=num;
			}
		}
		if (card[col+9]!=99&&card[col]!=99) {
			if (card[col+9]<card[col]) {
				num=card[col];
				card[col]=card[col+9];
				card[col+9]=num;
			}
		}
	}
}

struct card_st *getcard(connection *u)
{
	struct card_st *sweep=NULL;

	for (sweep=cards;sweep;sweep=sweep->next) {
		if (strcasecmp(u->name, sweep->name)==0) return sweep;
	}

	return NULL;
}

void buycard(connection *u, char *arg)
{
	char temp[4096];
	long bet=0;

	if (arg) {
		bet=atol(arg);
		if (bet<5 || bet>1000) {
			swrite(NULL,u,"[bingo] bets need to be between 5 and 1000 flimps.\n");
			return;
		}
		if (bet!=minbet) {
			if (minbet) {
				swrite(NULL,u,"[bingo] The bet value has already been set, just type buycard on it's own.\n");
				return;
			}
			minbet=bet;
		}
	}

	if (getcard(u)) {
		swrite(NULL,u,"[bingo] You already have a card\n");
		return;
	}

	if (current==0&&cards==NULL) {
		if (!minbet) minbet=5;
		nextgame=time(0)+(GAMEWAIT*60);
		sprintf(temp,"[bingo] Next game in %d minutes.\n", GAMEWAIT);
		sbingo(NULL, temp);
		sprintf(temp,"[bingo] ^CCards will cost: ^G%ld^N flimps.\n", minbet);
		sbingo(NULL, temp);
	}

	create_card(u->name);
	swrite(NULL,u,"[bingo] Card created\n");
	sprintf(temp, "[bingo] %s buys a bingo card.\n",u->name);
	sbingo(u,temp);
}

void showcard(connection *u, char *arg)
{
	struct card_st *mycard=NULL;
	unsigned char num=0;
	short pos=0;
	short col=0;
	short row=0;
	char numstr[10];
	char temp[4096];
	char line[128];

	temp[0]='\0';

	mycard=getcard(u);

	if (!mycard) {
		swrite(NULL,u,"[bingo] You are not playing\n");
		return;
	}

	swrite(NULL,u,"Your bingo card...\n");
	for (row=0;row<3;row++) {
		for (col=0;col<9;col++) {
			pos=col+(row*9);
			if (mycard->card[pos]!=99) {
				num=mycard->card[pos];
				if (num>100) {
					sprintf(numstr, "--");
				}
				else
				{
					sprintf(numstr, "%02d", num);
				}
			}
			else
			{
				sprintf(numstr, "--");
			}
			sprintf(line, " %s ", numstr);
			strcat(temp, line);
		}
		strcat(temp, "\n");
	}

	swrite(NULL,u,temp);
}

int baddaub(struct card_st *mycard)
{
	struct card_st *sweep=NULL;
	struct card_st *next=NULL;
	struct card_st *last=NULL;

	mycard->baddaub++;

	if (mycard->baddaub<5) return 0;

	if (mycard==cards) {
		cards=mycard->next;
	}
	else
	{
		for (sweep=cards;sweep;sweep=next) {
			next=sweep->next;
			if (sweep!=mycard) {
				last=sweep;
				continue;
			}
			last->next=sweep->next;
			break;
		}
	}
	free(mycard);
	return 1;
}

void daub(connection *u, char *arg)
{
	struct card_st *mycard=NULL;
	char temp[4096];
	short found=-1;
	short count=0;
	unsigned char  num=0;

	if (!arg) {
		swrite(NULL,u,"[bingo] Specify a number\n");
		return;
	}

	mycard=getcard(u);

	if (!mycard) {
		swrite(NULL,u,"[bingo] You are not playing\n");
		return;
	}

	if (mycard->game==1) {
		swrite(NULL,u,"[bingo] You need to wait for the game to restart\n");
		return;
	}

	num=atoi(arg);

	if (num<1||num>90) {
		swrite(NULL,u,"[bingo] Number must be between 1 and 90\n");
		return;
	}

	for (count=0;count<27;count++) {
		if (mycard->card[count]==num) found=count;
	}

	if (found==-1) {
		swrite(NULL,u,"[bingo] You do not have that number (or it is aleady daubed)\n");
		if (baddaub(mycard)) {
			swrite(NULL,u,"[bingo] Too many bad daubs, your card is torn up.\n");
		}
		return;
	}

	if (numbers[num-1]!='X') {
		swrite(NULL,u,"[bingo] That number has not yet been called.\n");
		if (baddaub(mycard)) {
			swrite(NULL,u,"[bingo] Too many bad daubs, your card is torn up.\n");
		}
		return;
	}

	mycard->card[found]+=100;
	sprintf(temp,"[bingo] %d daubed\n",num);
	swrite(NULL,u,temp);
	showcard(u,NULL);
}

void finish_game(connection *u)
{
	char temp[4096];
	long cash=0;
	char *muff=NULL;

	if (u) {
		sprintf(temp,"[bingo] %s has a full house!\n", u->name);
		sbingo(NULL, temp);
		muff=get_attribute(u->object,"money");
		if (muff) {
			cash=atol(muff);
			FREE(muff);
		}	
		cash+=prize;
		sprintf(temp,"%ld",cash);
		set_attribute(u->object,"money",temp);
		sprintf(temp,"[bingo] %ld flimps prize money goes to %s.\n",prize,u->name);
		sbingo(NULL, temp);
	}
	else
	{
		sbingo(NULL, "[bingo] Game ended with no winners.\n");
	}

	current=0;
	remove_current();
	if (cards) {
		nextgame=time(0)+(GAMEWAIT*60);
		sprintf(temp,"[bingo] Next game in %d minutes.\n", GAMEWAIT);
		sbingo(NULL, temp);
		sprintf(temp,"[bingo] ^CCards will cost: ^G%ld^N flimps.\n", minbet);
		sbingo(NULL, temp);
	}
}

void house(connection *u, char *arg)
{
	struct card_st *mycard=NULL;
	short count=0;

	mycard=getcard(u);

	if (!mycard) {
		swrite(NULL,u,"[bingo] You are not playing\n");
		return;
	}

	if (mycard->game==1) {
		swrite(NULL,u,"[bingo] You need to wait for the game to restart\n");
		return;
	}

	for (count=0;count<27;count++) {
		if (mycard->card[count]<99) {
			swrite(NULL,u,"[bingo] Oh no you don't\n");
			return;
		}
	}

	finish_game(u);
}

void warn_game()
{
	char temp[4096];
	sprintf(temp,"[bingo] 2 minute warning for next game. (card cost: ^G%ld^N flimps)\n", minbet);
	sbingo(NULL, temp);
	warned=1;
}

void new_game()
{
	connection *sweep=NULL;
	long cash=0;
	char temp[4096];
	short count=0;
	struct card_st *sweep2=NULL;
	char *muff=NULL;

	warned=0;
	current=1;

	for (sweep2=cards;sweep2;sweep2=sweep2->next) {
		count++;
	}

	if (count==1) {
		sbingo(NULL, "[bingo] Game cancelled, not enough players.\n");
		free(cards);
		cards=NULL;
		minbet=0;
		current=0;
		return;
	}

	init_game();

	for (sweep=users;sweep;sweep=sweep->next) {
		if (sweep->quiet) continue;
		if (!getcard(sweep)) continue;

		cash=0;
		muff=get_attribute(sweep->object,"money");
		if (muff) {
			cash=atol(muff);
			FREE(muff);
		}	
		cash-=minbet;
		sprintf(temp,"%ld",cash);
		set_attribute(sweep->object,"money",temp);
	}

	prize=(count * minbet) * count;
	sbingo(NULL, "[bingo] OK, new game, cards at the ready\n");
	sprintf(temp, "[bingo] ^CPrize money: ^G%ld^N flimps.\n", prize);
	sbingo(NULL, temp);
	for (sweep2=cards;sweep2;sweep2=sweep2->next) {
		sprintf(temp,"[bingo] %s is playing.\n", sweep2->name);
		sbingo(NULL, temp);
	}
	sbingo(NULL, "[bingo] Right, lets go...\n");
	lastcall=time(0);
	minbet=0;
}

void call_game()
{
	short numl=0;
	short pos=0;
	short num=-1;
	short count=0;
	short inpos=0;
	char temp[4096];

	if (numleft()!=0) {
		if (time(0)-lastcall<=CALLWAIT) return;
	}
	else
	{
		if (time(0)-lastcall<=LASTWAIT) return;
	}

	numl=numleft();

	if (numl==0) {
		finish_game(NULL);
		return;
	}

	pos=psuedo_rand(numl);

	for (count=0;count<90;count++) {
		if (numbers[count]=='X') continue;
		inpos++;
		if (inpos==pos) {
			num=count;
			break;
		}
	}

	if (num==-1) {
		sbingo(NULL, "[bingo] Game error, ending game\n");
		finish_game(NULL);
		return;
	}

	numbers[num]='X';

	sprintf(temp, "[bingo] ^C%s, number^Y %d^N.\n", calls[num+1], num+1);
	pbingo(NULL, temp);

	lastcall=time(0);
}

void called(connection *u, char *arg)
{
	char temp[4096];
	char temp2[4096];
	short count=0;
	short count2=0;
	struct card_st *mycard=NULL;

	mycard=getcard(u);

	if (!mycard) {
		swrite(NULL,u,"[bingo] You are not playing\n");
		return;
	}

	swrite(NULL,u,"Numbers shown have already been called.\n");
	for (count=1;count<=10;count++) {
		temp[0]='\0';
		for (count2=0;count2<9;count2++) {
			if (numbers[((count2*10)+(count-1))]=='X') {
				sprintf(temp2," %02d ", (count2*10)+count);
				strcat(temp, temp2);
			}
			else
			{
				strcat(temp, " -- ");
			}
		}
		strcat(temp, "\n");
		swrite(NULL,u,temp);
	}
}

void tick()
{
	if (current) {
		call_game();
	}
	else
	{
		if (!cards) return;
		if (time(0) > nextgame) {
			new_game();
		}
		else if ((time(0) > nextgame-120)&&(!warned)) {
			warn_game();
		}
	}
}

int init()
{
	add_command("buycard",&buycard,NULL,NULL);
	add_command("card",&showcard,NULL,NULL);
	add_command("daub",&daub,NULL,NULL);
	add_command("house",&house,NULL,NULL);
	add_command("called",&called,NULL,NULL);
	nextgame=time(0)+(GAMEWAIT*60);
	minbet=0;
	init_game();
	return 1;
}

int uninit()
{
	remove_command("card",NULL);
	remove_command("daub",NULL);
	remove_command("buycard",NULL);
	remove_command("house",NULL);
	remove_command("called",NULL);
	return 1;
}

char *version()
{
	return "Bingo (c) 2005 Karl Bastiman $Revision: 2.8 $";
}
