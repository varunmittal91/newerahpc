INCLUDE_PATH := -I./include

EXTRA_CFLAGS_SERVER := -Dnewerahpc -g $(INCLUDE_PATH) -Ddebug

all:server

server:clean_lib
	rm bin/libnewera_server.a
	cd src/grid && make
	cd src/http && make
	cd src/general && make
	cd src/tcp && make
	cd src/thread && make
	cd src/memory && make
	ar -cvq bin/libnewera_server.a src/grid/*.o src/http/*.o src/general/*.o src/tcp/*.o src/thread/*.o src/memory/*.o
	g++ server.cpp -L./bin -L./mpix/bin -lnewera_server -lpthread -o bin/server -I./include -g -ldl
	
server_only:
	g++ test.cpp -L./bin -L./mpix/bin -lnewera_server -lpthread -o bin/server -I./include -g

clean_lib:
	touch bin/libnewera_server.a

clean:
	cd src/grid && make clean
	cd src/http && make clean
	cd src/general && make clean
	cd src/tcp && make clean
	cd src/thread && make clean
	cd src/memory && make clean

mpi:
	cd mpix && make mpi

install:
	cp bin/libnewera_server.a /usr/lib/
	mkdir /usr/include/newerahpc
	cp include/* /usr/include/newerahpc

uninstall:
	rm /usr/lib/libnewera_server.a
	rm -rf /usr/include/newerahpc
