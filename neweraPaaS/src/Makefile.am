SUBDIRS = lxc_interface

lib_LTLIBRARIES          = libneweraPaaS.la
libneweraPaaS_la_SOURCES = 
libneweraPaaS_la_LIBADD  = lxc_interface/liblxc_interface.la
libneweraPaaS_la_LDFLAGS = -version-info 2:90 -ldl -lpthread -lneweraHPC 
libneweraPaaS_la_CFLAGS  = $(AM_CFLAGS)

