ACLOCAL_AMFLAGS= -I m4

SUBDIRS = src

bin_PROGRAMS   = server test_client

server_SOURCES = server.cpp
server_LDADD   = src/libneweraHPC_grid.la -ldl 
server_LDFLAGS = -lneweraHPC

test_client_SOURCES = test_client.cpp
test_client_LDADD   = src/libneweraHPC_grid.la -ldl 
test_client_LDFLAGS = -lneweraHPC

library_includedir      = $(includedir)/neweraHPC
library_include_HEADERS = include/*.h

dist_doc_DATA = README

src/libneweraHPC.la:
	cd src && make
   
