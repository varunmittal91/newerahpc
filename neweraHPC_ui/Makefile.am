ACLOCAL_AMFLAGS= -I m4

SUBDIRS = src

bin_PROGRAMS = main
main_SOURCES = main.cpp src/neweraHPC_ui.la
main_LDADD   = src/libneweraHPC_ui.la -ldl -lneweraHPC

dist_doc_DATA = README

src/libneweraHPC_ui.la:
	cd src && make
   
