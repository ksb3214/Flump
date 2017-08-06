/* $Id: commands.h,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stdio.h>

struct mode_s {
	char *prompt;
};
typedef struct mode_s mode;

struct command_s {
	char *name;
	void (*function)(connection *u,char *args);
	short type;
	char *alias;
	mode *commandmode;
	struct command_s *next;
	char flags[255];  /* bit crap but easy to read */
};
typedef struct command_s command;

struct lib_s {
	char *filename;
	int (*init)();
	int (*uninit)();
	void (*tick)();
	void (*event)(connection *u,short id,char *passed,char **ret);
	void *handle;
	char *(*version)();
	struct lib_s *next;
};
typedef struct lib_s lib;
	
extern void set_command_flag(char *, unsigned char);
extern char get_command_flag(char *, unsigned char);
extern void run_command(connection *, char *);
extern void add_command(char *, void (*)(connection *,char *), char *, mode *);
extern void clear_commands();
extern int load_dll(char *);
extern int unload_dll(lib *, int);
extern void clear_dlls();
extern void reload_dll(lib *);
extern void raise_event(connection *,short, char *, char **);
extern void remove_command(char *,mode *);
extern int is_command(char *cmd);
extern int accessok(command *, connection *);
extern void listcommands(connection *, char *);
extern mode *createmode();

extern command *commands;
extern lib *libs;

/* command flags */
#define C_REPEAT	((unsigned char)1)
#define C_NOSHORT	((unsigned char)2)
/*****************/
#endif
