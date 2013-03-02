SUBDIRS = network general poll ai system

lib_LTLIBRARIES         = libneweraHPC.la
libneweraHPC_la_SOURCES = neweraHPC.cpp 
libneweraHPC_la_LIBADD  = network/libnetwork.la general/libgeneral.la poll/libpoll.la ai/libai.la system/libsystem.la
libneweraHPC_la_LDFLAGS = -version-info 2:90 -ldl -lpthread 
libneweraHPC_la_CFLAGS  = $(AM_CFLAGS)

