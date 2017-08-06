/* $Id: socket.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _SOCKET_H_
#define _SOCKET_H_


#define MAX_BUFFER 3000
#define MAX_NAME 64

struct connection_s {
	int fd;
	unsigned long object;
	int quickdis; /* if set disconnect will do nothing but remove */
	void (*function)(struct connection_s *u,char *arg);
	time_t timestamp;
	char buffer[MAX_BUFFER];
	char tmpbuf[MAX_BUFFER];
	int bufpos;
	char name[MAX_NAME];
	struct connection_s *next;
	int quiet;
	time_t last_command;
	time_t logged_in;
	time_t last_spod;
	time_t cspod;
	time_t ctruespod;
	time_t last_sync;
	struct mode_s *usermode;
	char *this_command;
	void *prev_command; /* We cant use command * here ;( */
	char *lastarg;
	int disconnect; /* if set to 1 flump will force disconnect */
	int destroy;
	char *tellbuf[10];
	char ttype[255];
	int theight;
	int twidth;
};
typedef struct connection_s connection;

extern void init_main_socket();
extern void shutdown_main_socket();
extern void accept_connection();
extern void close_connections();
extern int input(connection *);
extern void swrite(connection *,connection *,char *);
extern void disconnect(connection *);
extern void sroom(connection *, char *);
extern void swall(connection *, char *);
extern void swrite(connection *, connection *, char *);
extern void showprompt(connection *);
extern void ping(connection *);
void wsock(int, char *);

extern int main_descriptor;
extern connection *users;

#endif
