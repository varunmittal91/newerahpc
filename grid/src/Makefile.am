SUBDIRS = grid grid_client grid_interface

lib_LTLIBRARIES              = libneweraHPC_grid.la
libneweraHPC_grid_la_SOURCES = grid.cpp
libneweraHPC_grid_la_LIBADD  = grid/libgrid.la grid_client/libgrid_client.la grid_interface/libgrid_interface.la
libneweraHPC_grid_la_LDFLAGS = -version-info 2:90 -ldl -lpthread -lneweraHPC 
libneweraHPC_grid_la_CFLAGS  = $(AM_CFLAGS)

