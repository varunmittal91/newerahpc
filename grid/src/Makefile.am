SUBDIRS = grid

lib_LTLIBRARIES              = libneweraHPC_grid.la
libneweraHPC_grid_la_SOURCES = grid.cpp
libneweraHPC_grid_la_LIBADD  = grid/libgrid.la
libneweraHPC_grid_la_LDFLAGS = -version-info 2:90 -ldl -lpthread 
libneweraHPC_grid_la_CFLAGS  = $(AM_CFLAGS)

