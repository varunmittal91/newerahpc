all:
	g++ plugin.cpp -o bin/plugin.o -c
	g++ bin/*.o -o bin/libplugin.so -shared -ldl -dynamiclib -rdynamic -fPIC -shared -lnewera_server
	
clean:
	rm bin/*
