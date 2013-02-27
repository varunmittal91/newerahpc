ACLOCAL_AMFLAGS= -I m4

SUBDIRS = src

bin_PROGRAMS   = server
server_SOURCES = server.cpp src/neweraHPC.la
server_LDADD   = src/libneweraHPC_grid.la -ldl 
server_LDFLAGS = -lneweraHPC

library_includedir      = $(includedir)/neweraHPC
library_include_HEADERS = include/*.h

dist_doc_DATA = README

src/libneweraHPC.la:
	cd src && make
   
