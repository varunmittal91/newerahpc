SUBDIRS = general web_interface

lib_LTLIBRARIES       = libjarvis.la
libjarvis_la_SOURCES  = jarvis.cpp jarvis_data.cpp grid_plugin.cpp
libjarvis_la_LIBADD   = general/libgeneral.la web_interface/libweb_interface.la
libjarvis_la_LDFLAGS  = -lneweraHPC_grid -lneweraHPC -ldl -always-shared $(neweraHPC_LIBS)
libjarvis_la_CPPFLAGS = $(neweraHPC_CFLAGS)
