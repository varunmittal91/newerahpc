SUBDIRS = src

bin_PROGRAMS = main jarvis_grid jarvis_grid_client

main_SOURCES  = main.cpp src/libjarvis.la
main_LDADD    = src/libjarvis.la 
main_LDFLAGS  = -lneweraHPC_grid -ldl -lneweraHPC $(neweraHPC_LIBS)
main_CPPFLAGS = $(neweraHPC_CFLAGS)

jarvis_grid_SOURCES  = jarvis_grid.cpp
jarvis_grid_LDADD    = src/libjarvis.la
jarvis_grid_CPPFLAGS = $(neweraHPC_CFLAGS)

jarvis_grid_client_SOURCES = jarvis_grid_client.cpp
