ACLOCAL_AMFLAGS= -I m4

bin_PROGRAMS = neweraPaaS

neweraPaaS_SOURCES  = neweraPaaS.cpp
neweraPaaS_LDADD    = src/libneweraPaaS.la -ldl 
neweraPaaS_CPPFLAGS = $(neweraHPC_CFLAGS)
neweraPaaS_LDFLAGS  = $(neweraHPC_LIBS)

