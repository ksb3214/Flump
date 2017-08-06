#include "flump.h"

/* p1 is always blue, p2 is red */
struct c4board_st {
	char p1[255];
	char p2[255];
	char go; /* 0 game not confirmed, 1 - p1's go, 2 - p2's go */
	char board[7][6]; /* 0 - empty, 1 - p1, 2 - p2 */
	struct c4board_st *next;
};

struct c4board_st *c4b=NULL;
int last=0;
int same=0;

struct c4board_st *add_game(connection *p1, connection *p2)
{
	struct c4board_st *tmp=NULL;
	struct c4board_st *tmp2=NULL;
	int count=0;
	int count2=0;

	tmp=(struct c4board_st *)malloc(sizeof(struct c4board_st));

	tmp->next=NULL;
	strcpy(tmp->p1, p1->name);
	strcpy(tmp->p2, p2->name);
	tmp->go=0;
	for (count=0; count<7; count++) {
		for (count2=0; count2<6; count2++) {
			tmp->board[count][count2]=0;
		}
	}

	if (!c4b) {
		c4b=tmp;
	}
	else
	{
		for (tmp2=c4b;tmp2->next;tmp2=tmp2->next);
		tmp2->next=tmp;
	}	
	return tmp;
}

void killgame(struct c4board_st *cur)
{
	struct c4board_st *tmp=NULL, *last=NULL;

	if (cur==c4b) {
		c4b=cur->next;
		free(cur);
		return;
	}

	last=c4b;
	for (tmp=c4b;tmp;tmp=tmp->next) {
		if (tmp==cur) {
			last->next=cur->next;
			free(cur);
			return;
		}
		last=tmp;
	}
}

struct c4board_st *find_game(connection *u)
{
	struct c4board_st *tmp=NULL;

	for (tmp=c4b;tmp;tmp=tmp->next) {
		if (strcasecmp(tmp->p1,u->name)==0) return tmp;
		if (strcasecmp(tmp->p2,u->name)==0) return tmp;
	}

	return NULL;
}

int addtoken(connection *u, struct c4board_st *cur, int pos)
{
	int count=0;

	if (cur->board[pos-1][5]!=0) return 0;

	for (count=0; count<6; count++) {
		if (cur->board[pos-1][count]==0) {
			cur->board[pos-1][count]=strcasecmp(u->name,cur->p1)==0?1:2;
			return 1;
		}
	}	
	return 0;
}

void outboard(connection *c, struct c4board_st *cur)
{
	int count=0;
	int count2=0;
	char temp[4096];

	swrite(NULL,c,"[c4] ^GBoard...^N\n");
	swrite(NULL,c,"[c4]   1 2 3 4 5 6 7\n");	
	for (count2=5; count2>=0; count2--) {
		sprintf(temp,"[c4] | ");
		for (count=0; count<7; count++) {
			switch (cur->board[count][count2]) {
				case 0 : strcat(temp, ". "); break;
				case 1 : strcat(temp, "^BO^N "); break;
				case 2 : strcat(temp, "^RO^N "); break;
				default : strcat(temp, ". "); break;
			}	
		}
		strcat(temp, "|\n");
		swrite(NULL,c,temp);
	}
	swrite(NULL,c,"[c4]  ---------------\n");	
}

int check_ele(struct c4board_st *cur, int x, int y)
{
	if (cur->board[x][y]==last) {
		same++;
	}
	else
	{
		same=0;
		last=cur->board[x][y];
	}
	if (same==3&&last!=0) {
		return last;
	}
	return 0;
}

int check_win(struct c4board_st *cur)
{
	int x=0, y=0, xaxis=0, yaxis=0;
	int res=0;
	
	/* horizontal */

	for (y=0; y<6; y++) {
		same=0; last=0;
		for (x=0;x<7;x++) {
			res=check_ele(cur,x,y);
			if (res) printf("here 1\n");
			if (res) return res;
		}
	}

	/* vertical */

	for (x=0; x<7; x++) {
		same=0; last=0;
		for (y=0; y<6; y++) {
			res=check_ele(cur,x,y);
			if (res) printf("here 2\n");
			if (res) return res;
		}
	}	

	/* Diagonal */
	for (xaxis=0;xaxis<7;xaxis++) {
		same=0; last=0;
		for (x=xaxis,y=0;x<7&&y<6;x++,y++) {
			res=check_ele(cur,x,y);
			if (res) printf("here 3\n");
			if (res) return res;
		}
	}

	for (yaxis=0;yaxis<6;yaxis++) {
		same=0; last=0;
		for (x=0,y=yaxis;x<7&&y<6;x++,y++) {
			res=check_ele(cur,x,y);
			if (res) printf("here 4\n");
			if (res) return res;
		}
	}

	for (xaxis=0;xaxis<7;xaxis++) {
		same=0; last=0;
		for (x=xaxis,y=0;x>=0&&y<6;x--,y++) {
			res=check_ele(cur,x,y);
			if (res) printf("here 5\n");
			if (res) return res;
		}
	}

	for (yaxis=0;yaxis<6;yaxis++) {
		same=0; last=0;
		for (x=0,y=yaxis;x<7&&y>=0;x++,y--) {
			res=check_ele(cur,x,y);
			if (res) printf("here 6\n");
			if (res) return res;
		}
	}

	same=0; last=0;
	for (x=0; x<7; x++) {
		for (y=0; y<6; y++) {
			if (cur->board[x][y]==0) {
				return 0;
			}	
		}
	}	

	/* a draw */
	return 3;
}

/* ok...
**
** c4 name - starts a game or confirms a challange
** c4 -name - denies a challenge
** c4 1-7 - drops a piece on your go
** c4 quit - stops the game
*/
void c4(connection *u, char *arg)
{
	struct c4board_st *cur=NULL, *tmp=NULL;
	connection *chal=NULL;
	char temp[4096];
	int pos=0;
	int res=0;
	char *dat=NULL;
	unsigned long c4win=0;

	cur=find_game(u);

	temp[0]='\0';

	if (!arg) {
		if (!cur) {
			swrite(NULL,u,"[c4] To challenge a user type 'c4 username'.\n");
			return;
		}
		outboard(u,cur);
		return;
	}

	chal=getubyname(u,arg,temp);

	if (!chal) {
		if (temp[0]!='\0' && temp[0]!='N') /* utter shite coding */
		{
			swrite(NULL,u,"[c4] ");
			swrite(NULL,u,temp);
			return;
		}
	}
	else
	{
		if (chal==u) {
			swrite(NULL,u,"[c4] You cannot play yourself.\n");
			return;
		}

		if (cur) {
			if (cur->go>0) {
				swrite(NULL,u,"[c4] Your game is already in progress.\n");
				return;
			}
			if (strcasecmp(cur->p1,chal->name)==0) {
				cur->go=1;
				sprintf(temp,"[c4] You accept the challenge from %s, you are playing as ^Rred^N.\n[c4] %s goes first.\n",cur->p1,cur->p1);
				swrite(NULL,u,temp);
				outboard(u,cur);
				sprintf(temp,"[c4] %s accepts your challenge, you are playing as ^Bblue^N.\n[c4] It is your go first.\n",u->name);
				swrite(NULL,chal,temp);
				outboard(chal,cur);
				return;
			}
			if (strcasecmp(cur->p1,u->name)==0) {
				sprintf(temp,"[c4] You have already challenged %s.\n",cur->p2);
				swrite(NULL,u,temp);
				return;
			}
			if (strcasecmp(cur->p2,u->name)==0) {
				sprintf(temp,"[c4] You have already been challenged by %s.\n",cur->p1);
				swrite(NULL,u,temp);
				return;
			}	
		}
		else
		{
			cur=find_game(chal);
			if (cur) {
				swrite(NULL,u,"[c4] They are already involved in a game.\n");
				return;
			}
			cur=add_game(u,chal);
			sprintf(temp,"[c4] You challenge %s to a game of connect 4.\n",chal->name);
			swrite(NULL,u,temp);
			sprintf(temp,"[c4] %s challenges %s.\n",u->name,chal->name);
			sroom(u,temp);
			sprintf(temp,"[c4] Accept with 'c4 %s', refuse with 'c4 deny'\n",u->name);
			swrite(NULL,chal,temp);
		}	
		return;
	}	

	/* any arguments that do not require a current game need to be here */
	if (strcasecmp(arg,"list")==0) {
		if (!c4b) {
			swrite(NULL,u,"[c4] No games currently in progress.\n");
			return;
		}	
		for (tmp=c4b;tmp;tmp=tmp->next) {
			sprintf(temp,"[c4] %s v's %s (%s)\n",tmp->p1, tmp->p2, tmp->go==0?"Awaiting confirmation":"Started");
			swrite(NULL,u,temp);
		}
		return;
	}	
			
	if (!cur) {
		swrite(NULL,u,"[c4] You need to get yourself a game dude.\n");
		return;
	}	

	if (strcasecmp(arg,"deny")==0) {
		if (cur->go!=0) {
			swrite(NULL,u,"[c4] Your game has started already, too late to deny.\n");
			return;
		}

		if (strcasecmp(cur->p2,u->name)!=0) {
			swrite(NULL,u,"[c4] You cannot back out of your own challenge, try 'c4 quit'.\n");
			return;
		}

		sprintf(temp,"[c4] You deny a challenge from %s.\n",cur->p1);
		swrite(NULL,u,temp);
		chal=getubyname(NULL,cur->p1,temp);
		if (chal) {
			sprintf(temp,"[c4] %s refuses your challenge.\n",cur->p2);
			swrite(NULL,chal,temp);
		}	
		killgame(cur);
		return;
	}	
			
	if (strcasecmp(arg,"quit")==0) {
		chal=getubyname(NULL,strcasecmp(u->name,cur->p1)==0?cur->p2:cur->p1,temp);
		sprintf(temp,"[c4] You quit playing\n");
		swrite(NULL,u,temp);
		if (chal) {
			sprintf(temp,"[c4] %s quits the game. (report cheating to an admin)\n",u->name);
			swrite(NULL,chal,temp);
		}	
		killgame(cur);
		return;
	}	
			
	switch(cur->go) {
		case 0 : swrite(NULL,u,"[c4] The game hasn't started yet.\n");
			 return;
			 break;
		case 1 : if (strcasecmp(cur->p1,u->name)!=0) {
			 	swrite(NULL,u,"[c4] It isn't your turn yet.\n");
			 	return;
			 }
			 break;
		case 2 : if (strcasecmp(cur->p2,u->name)!=0) {
			 	swrite(NULL,u,"[c4] It isn't your turn yet.\n");
			 	return;
			 }
			 break;
		default: swrite(NULL,u,"[c4] Unknown error.\n"); return; break;
	}	

	/* at this point we known a game is going on and it is the players 
	turn, so lets see if it's a column, any command matches need to go
	first */

	pos=atoi(arg);
	if (pos<1 || pos >7) {
		swrite(NULL,u,"[c4] Invalid column, please specify 1 to 7.\n");
		return;
	}

	/* we have the correct column */

	if (!addtoken(u,cur,pos)) {
		swrite(NULL,u,"[c4] Column is full, try again.\n");
		return;
	}

	sprintf(temp,"[c4] You place a token in slot %d.\n",pos);
	swrite(NULL,u,temp);
	outboard(u,cur);
	chal=getubyname(NULL,cur->go==1?cur->p2:cur->p1,temp);
	if (!chal) {
		sprintf(temp,"[c4] %s has left the talker, either wait for them to return or 'c4 quit'.\n",cur->go==1?cur->p2:cur->p1);
		swrite(NULL,u,temp);
	}
	else
	{
		sprintf(temp,"[c4] %s places a token in slot %d.\n",u->name,pos);
		swrite(NULL,chal,temp);
		outboard(chal,cur);
		res=check_win(cur);
		if (res==3) {
			sprintf(temp,"[c4] DRAAAAAAAAAAAAWWW\n");
			swrite(NULL,u,temp);
			swrite(NULL,chal,temp);
			killgame(cur);
			c4win=0;
			dat=get_attribute(u->object,"c4draw");
			if (dat) {
				c4win=atol(dat);
				FREE(dat);
			}	
			c4win++;
			sprintf(temp,"%ld",c4win);
			set_attribute(u->object,"c4draw",temp);
			c4win=0;
			dat=get_attribute(chal->object,"c4draw");
			if (dat) {
				c4win=atol(dat);
				FREE(dat);
			}	
			c4win++;
			sprintf(temp,"%ld",c4win);
			set_attribute(chal->object,"c4draw",temp);
			return;
		}
		if (res) {
			swrite(NULL,u,"[c4] YOU WIN!!!\n");
			sprintf(temp,"[c4] %s wins, against %s.\n",u->name,chal->name);
			sroom(u,temp);
			killgame(cur);
			c4win=0;
			dat=get_attribute(u->object,"c4win");
			if (dat) {
				c4win=atol(dat);
				FREE(dat);
			}	
			c4win++;
			sprintf(temp,"%ld",c4win);
			set_attribute(u->object,"c4win",temp);
			c4win=0;
			dat=get_attribute(u->object,"c4winrow");
			if (dat) {
				c4win=atol(dat);
				FREE(dat);
			}	
			c4win++;
			sprintf(temp,"%ld",c4win);
			set_attribute(u->object,"c4winrow",temp);
			c4win=0;
			dat=get_attribute(chal->object,"c4lose");
			if (dat) {
				c4win=atol(dat);
				FREE(dat);
			}	
			c4win++;
			sprintf(temp,"%ld",c4win);
			set_attribute(chal->object,"c4lose",temp);
			sprintf(temp,"0");
			set_attribute(chal->object,"c4winrow",temp);
			return;
		}	
	}	

	if (cur->go==1) cur->go=2; else cur->go=1;
}

int init()
{
	add_command("c4",&c4,NULL,NULL);
	return 1;
}

int uninit()
{
	remove_command("c4",NULL);
	return 1;
}

char *version()
{
	return "Connect 4 (c) 2004 Karl Bastiman $Revision: 2.0 $";
}
