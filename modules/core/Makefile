# $Id: Makefile.in,v 2.0 2005/07/05 16:45:44 ksb Exp $ 
# $Revision: 2.0 $ 


INSTALLPATH?=${HOME}
INSTALL=/usr/bin/install -c
RM=/bin/rm -f
CC=gcc

LDFLAGS= -lmysqlclient -ldl   -L/usr/local/lib -L/usr/local/lib/mysql -pthread -L. -export-dynamic
CFLAGS=  -I/usr/local/include -pthread -I../../includes -ansi -ggdb3 -fPIC

MODULES=login.so

.SUFFIXES: .c .so

all: ${MODULES}

.c.so: 
	${CC} -shared $(CFLAGS) -o $@ $<
