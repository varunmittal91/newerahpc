ACLOCAL_AMFLAGS= -I m4

SUBDIRS = src

bin_PROGRAMS   = parser

parser_SOURCES = parser.cpp
parser_LDADD   = src/libneweraHPC_crawler.la -ldl 
parser_LDFLAGS = -lneweraHPC -ljarvis -lneweraHPC_grid

library_includedir      = $(includedir)/neweraHPC
library_include_HEADERS = include/*.h

dist_doc_DATA = README

src/libneweraHPC_crawler.la:
	cd src && make
   
