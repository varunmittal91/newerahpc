#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([neweraHPC], [2.90], [varunmittal91@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([server.cpp])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_ARG_ENABLE(debug, "turn on debug flag", CFLAGS = "$CFLAGS -debug")

# Checks for libraries.
AC_CHECK_LIB([pthread],[pthread_create],[],[AC_MSG_ERROR([libpthread not found])])

# Checks for header files.
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([dlfcn.h])

# Add HAVE_header-files to project build
AC_CONFIG_HEADERS([config.h])

# Check for optional arguments
AC_ARG_ENABLE([tests],[  --enable-tests Build test files for various algorithms and features used in the system],[tests=yes])
AM_CONDITIONAL([tests_enable], [test x$tests = xtrue])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
LD = g++
LT_INIT()
LT_SUPPORTED_TAG(CXX)

AC_CONFIG_FILES([Makefile]
	        [src/Makefile]
                [src/general/Makefile]
	        [src/network/Makefile]
	        [src/network/web_server/Makefile]
		[src/poll/Makefile]
		[src/ai/Makefile]
		[src/grid/Makefile]
		[src/system/Makefile]
                [tests/Makefile]
                [tests/general/Makefile]
                [tests/network/Makefile]
		[tests/grid/Makefile])
AC_OUTPUT
