# $Id: Makefile.in,v 2.1 2005/07/08 10:01:39 matt Exp $ 
# $Revision: 2.1 $ 


INSTALLPATH?=${HOME}
INSTALL=/usr/bin/install -c
RM=/bin/rm -f
CC=gcc

OBJS=main.o database.o config.o cache.o malloc.o debug.o socket.o commands.o misc.o rooms.o formatting.o modules.o signals.o data.o
SRCS=main.c database.c config.c cache.c malloc.c debug.c socket.c commands.c misc.c rooms.c formatting.c modules.c signals.c data.c

LDFLAGS= -lmysqlclient -ldl   -L/usr/local/lib -L/usr/local/lib/mysql -pthread -L.
CFLAGS=  -I/usr/local/include -pthread -I./includes -ansi -ggdb3

all: flump modules 

flump: ${OBJS} ${MODULES}
	${CC} -export-dynamic -o flump ${OBJS} ${LDFLAGS}

clean:
	${RM} -f *.o *.so flump
	${RM} -f modules/*/*.so

install:
	mkdir -p ${INSTALLPATH}/bin
	${INSTALL} flump ${INSTALLPATH}/bin

modules::
	cd modules && make all

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.
