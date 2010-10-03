MAIN_INCLUDE_FILES := ../include

all:
	g++ plugin.cpp -o bin/plugin.o -c -I$(MAIN_INCLUDE_FILES)
	g++ bin/*.o -o bin/libplugin.so -shared -ldl -dynamiclib -I$(MAIN_INCLUDE_FILES) -L../bin -lnewera_server
	
clean:
	rm bin/*
