# Process this file with autoconf to produce a configure script.

AC_INIT([Flump],[1.0],[flump@amber.org.uk])
AC_CONFIG_SRCDIR([cache.c])
AC_CANONICAL_HOST
AC_CONFIG_HEADER([includes/defs.h])

LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/local/lib/mysql"
CFLAGS="$CFLAGS -I/usr/local/include"
CPPFLAGS="$CPPFLAGS -I/usr/local/include"

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

# Checks for libraries.
AC_SEARCH_LIBS([socket],[socket],[],AC_MSG_ERROR([no socket support found]))
AC_SEARCH_LIBS([dlopen],[dl],[],AC_MSG_ERROR([no dynamic library support found]))
AC_SEARCH_LIBS([mysql_init], [mysqlclient], [], AC_MSG_ERROR([cannot find mysql]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h arpa/telnet.h signal.h errno.h dlfcn.h ctype.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h mysql/mysql.h pthread.h])
# AC_CHECK_HEADER([mysql/mysql.h],[],AC_MSG_ERROR([cannot find mysql headers]))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM


# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([bzero select strcasecmp strchr strncasecmp])

PTHREADS_CHECK
PTHREADS_ASSIGN_VARS
LDFLAGS="$LDFLAGS $ac_cv_pthreads_cflags"
AC_CONFIG_FILES([Makefile
                 modules/Makefile
                 modules/core/Makefile
                 modules/user/Makefile])
AC_OUTPUT
