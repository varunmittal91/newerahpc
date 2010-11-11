INCLUDE_PATH := -I./include

EXTRA_CFLAGS_SERVER := -Dnewerahpc -g $(INCLUDE_PATH) -Ddebug

server:clean
	rm bin/libnewera_server.a
	g++ src/grid/grid.cpp -c -o bin/grid.o $(EXTRA_CFLAGS_SERVER)
	g++ src/grid/grid_conf.cpp -c -o bin/grid_conf.o $(EXTRA_CFLAGS_SERVER)
	g++ src/grid/grid_plugin.cpp -c -o bin/grid_plugin.o $(EXTRA_CFLAGS_SERVER)
	g++ src/grid/grid_nxi.cpp -c -o bin/grid_nxi.o $(EXTRA_CFLAGS_SERVER)
	g++ src/grid/grid_dynamic.cpp -c -o bin/grid_dynamic.o $(EXTRA_CFLAGS_SERVER)
	g++ src/grid/grid_lng.cpp -c -o bin/grid_lng.o $(EXTRA_CFLAGS_SERVER) 
	g++ src/grid/grid_data.cpp -c -o bin/grid_data.o $(EXTRA_CFLAGS_SERVER)
	g++ src/grid/grid_task.cpp -c -o bin/grid_task.o $(EXTRA_CFLAGS_SERVER)
	g++ src/grid/grid_instruction.cpp -c -o bin/grid_instruction.o $(EXTRA_CFLAGS_SERVER)
	g++ src/thread/thread.cpp -c -o bin/thread.o $(EXTRA_CFLAGS_SERVER)
	g++ src/general/log.cpp -c -o bin/log.o $(EXTRA_CFLAGS_SERVER)
	g++ src/general/queue.cpp -c -o bin/queue.o $(EXTRA_CFLAGS_SERVER)
	g++ src/general/string.cpp -c -o bin/string.o $(EXTRA_CFLAGS_SERVER)
	g++ src/general/file.cpp -c -o bin/file.o $(EXTRA_CFLAGS_SERVER)
	g++ src/http/http.cpp -c -o bin/http.o $(EXTRA_CFLAGS_SERVER)
	g++ src/http/http_file.cpp -c -o bin/http_file.o $(EXTRA_CFLAGS_SERVER)
	g++ src/http/http_reply.cpp -c -o bin/http_reply.o $(EXTRA_CFLAGS_SERVER)
	g++ src/http/http_client.cpp -c -o bin/http_client.o $(EXTRA_CFLAGS_SERVER)
	g++ src/tcp/socket.cpp -c -o bin/socket.o $(EXTRA_CFLAGS_SERVER)
	g++ src/tcp/data_read.cpp -c -o bin/data_read.o $(EXTRA_CFLAGS_SERVER)
	g++ src/tcp/data_write.cpp -c -o bin/data_write.o $(EXTRA_CFLAGS_SERVER)
	g++ src/tcp/conn_database.cpp -c -o bin/conn_database.o $(EXTRA_CFLAGS_SERVER)
	ar -cvq bin/libnewera_server.a bin/*.o
	g++ server.cpp -L./bin -L./mpix/bin -lnewera_server -lpthread -o bin/server -I./include -g -ldl
	rm bin/*.o
	
server_only:
	g++ test.cpp -L./bin -L./mpix/bin -lnewera_server -lpthread -o bin/server -I./include -g

clean:
	touch bin/temp.o
	touch bin/libnewera_server.a
	touch bin/libnewera_client.a
	rm bin/*.o

mpi:
	cd mpix && make mpi

install:
	cp bin/libnewera_server.a /usr/lib/
	mkdir /usr/include/newerahpc
	cp include/* /usr/include/newerahpc

uninstall:
	rm /usr/lib/libnewera_server.a
	rm -rf /usr/include/newerahpc
