AC_INIT(neweraPaaS, 0.1, varunmittal91@gmail.com)
AC_PREREQ([2.59])

AM_INIT_AUTOMAKE([1.10 -Wall no-define])
AM_SILENT_RULES([yes])
AM_CONFIG_HEADER(config.h)

AC_CONFIG_SRCDIR([neweraPaaS.cpp])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([pthread],[pthread_create],[],[AC_MSG_ERROR([libpthread not found])])
#AC_CHECK_LIB([neweraHPC], [neweraHPC_init], [], [AC_MSG_ERROR([neweraHPC library not found])])

AC_ARG_WITH([neweraHPC-include-path], [AS_HELP_STRING([--with-neweraHPC-include-path],
    [location of the neweraHPC headers, defaults to /usr/include/neweraHPC])],
  [neweraHPC_CFLAGS="-I$withval"],
  [neweraHPC_CFLAGS="-I/usr/include/neweraHPC"])
AC_SUBST([neweraHPC_CFLAGS])

AC_ARG_WITH([neweraHPC-lib-path],
  [AS_HELP_STRING([--with-neweraHPC-lib-path], [location of the neweraHPC core libraries])],
  [neweraHPC_LIBS="-L$withval"],
  [neweraHPC_LIBS="-L/usr/lib"])
AC_SUBST([neweraHPC_LIBS])

# Checks for header files.
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([dlfcn.h])
AC_CHECK_HEADERS([neweraHPC.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
LT_INIT()
LT_SUPPORTED_TAG(CXX)

AC_OUTPUT([Makefile]
	  [src/Makefile]
	  [src/lxc_interface/Makefile])


