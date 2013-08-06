AC_INIT(neweraHPC_ui, 1, varunmittal91@gmail.com)


AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])
AM_CONFIG_HEADER(config.h)

AC_CONFIG_SRCDIR([main.cpp])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for header files.
AC_CHECK_HEADERS([neweraHPC/neweraHPC.h])
AC_CHECK_HEADERS([dlfcn.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
LT_INIT()
LT_SUPPORTED_TAG(CXX)

AC_OUTPUT([Makefile]
          [src/Makefile]
	  [src/apps/Makefile]
	  [src/webui_core/Makefile]
	  )
