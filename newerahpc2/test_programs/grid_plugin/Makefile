MPIX_INCLUDE = ../mpix/include
MPIX_LIB = ../mpix/bin

all:
	g++ plugin.cpp -o bin/plugin.o -c -I $(MPIX_INCLUDE)
	g++ bin/*.o -o bin/libplugin.so -shared -ldl -dynamiclib -rdynamic -fPIC -shared -lnewera_server -L$(MPIX_LIB) -lmpix
	
clean:
	rm bin/*
