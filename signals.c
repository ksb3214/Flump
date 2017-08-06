/* $Id: signals.c,v 2.0 2005/07/05 16:45:44 ksb Exp $ */
/* $Revision: 2.0 $ */

#include <flump.h>

#ifdef LINUX

char *signal_names[] = {
	"nothing",
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT",
	"SIGBUS",
	"SIGFPE",
	"SIGKILL",
	"SIGUSR1",
	"SIGSEGV",
	"SIGUSR2",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGSTKFLT",
	"SIGCHLD",
	"SIGCONT",
	"SIGSTOP",
	"SIGTSTP",
	"SIGTTIN",
	"SIGTTOU",
	"SIGURG",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGVTALRM",
	"SIGPROF",
	"SIGWINCH",
	"SIGIO",
	"SIGPWR",
	"SIGSYS"};
#else
char *signal_names[] = {
	"nothing",
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT",
	"SIGEMT",
	"SIGFPE",
	"SIGKILL",
	"SIGBUS",
	"SIGSEGV",
	"SIGSYS",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGURG",
	"SIGSTOP",
	"SIGTSTP",
	"SIGCONT",
	"SIGCHLD",
	"SIGTTIN",
	"SIGTTOU",
	"SIGIO",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGVTALRM",
	"SIGPROF",
	"SIGWINCH",
	"SIGINFO",
	"SIGUSR1",
	"SIGUSR2"};
#endif

void do_nothing( int dummy )
{
	printf("Signal Ignored.\n"); 
}

void save_and_quit(int dummy)
{
	exit(0);
}

void handle(int sig)
{
	char temp[TMP_SIZE];
	connection *sweep=NULL;

	switch(sig)
	{
		case SIGKILL:
		case SIGTERM:
		case SIGSEGV:
		case SIGINT:
		case SIGBUS: save_and_quit(sig);
			     break;
		case SIGWINCH:
			debug("WINCH handled");
			for (sweep=users;sweep;sweep=sweep->next) {
				sprintf(temp,"%c%c%c",IAC,DO,TELOPT_TTYPE);
				wsock(sweep->fd,temp);
				sprintf(temp,"%c%c%c",IAC,DO,TELOPT_NAWS);
				wsock(sweep->fd,temp);

			}
			break;
		default:
			printf("Caught signal %s (%d)\n",signal_names[sig],sig);
			do_nothing(sig);
			break;
	}
	fflush(stdout);
}

void init_sig()
{
	signal(SIGHUP,&handle);
	signal(SIGINT,&handle);
	signal(SIGQUIT,&handle);
	signal(SIGILL,&handle);
	signal(SIGTRAP,&handle);
	signal(SIGABRT,&handle);
	signal(SIGIOT,&handle);
	signal(SIGBUS,&handle);
	signal(SIGFPE,&handle);
	signal(SIGKILL,&handle);
	signal(SIGUSR1,&handle);
	signal(SIGSEGV,&handle);
	signal(SIGUSR2,&handle);
	signal(SIGPIPE,&handle);
	signal(SIGALRM,&handle);
	signal(SIGTERM,&handle);
#ifdef SIGSTKFLT
	signal(SIGSTKFLT,&handle);
#endif
	signal(SIGCHLD,&handle);
	signal(SIGCONT,&handle);
	signal(SIGSTOP,&handle);
	signal(SIGTSTP,&handle);
	signal(SIGTTIN,&handle);
	signal(SIGTTOU,&handle);
	signal(SIGURG,&handle);
	signal(SIGXCPU,&handle);
	signal(SIGXFSZ,&handle);
	signal(SIGVTALRM,&handle);
	signal(SIGPROF,&handle);
	signal(SIGWINCH,&handle);
	signal(SIGIO,&handle);
#ifdef SIGPWR
	signal(SIGPWR,&handle);
#endif
	signal(SIGSYS,&handle);
}

