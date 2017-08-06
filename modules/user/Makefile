# $Id: Makefile.in,v 2.2 2005/07/08 14:40:42 ksb Exp $ 
# $Revision: 2.2 $ 


INSTALLPATH?=${HOME}
INSTALL=/usr/bin/install -c
RM=/bin/rm -f
CC=gcc

LDFLAGS= -lmysqlclient -ldl   -L/usr/local/lib -L/usr/local/lib/mysql -pthread -L. -export-dynamic 
CFLAGS=  -I/usr/local/include -pthread -I../../includes -ansi -ggdb3 -fPIC

MODULES=rooms.so basic.so environment.so admin.so privs.so custom.so info.so auto.so spodtimes.so lists.so dns.so socials.so bugs.so radio.so motd.so connect4.so messages.so quotes.so news.so bingo.so money.so bling.so games.so webinfo.so

.SUFFIXES: .c .so

all: ${MODULES}

.c.so: 
	${CC} -shared $(CFLAGS) -o $@ $< 
