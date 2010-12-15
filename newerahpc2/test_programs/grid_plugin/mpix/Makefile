all:clean mpi test

mpi:clean
	g++ src/init.cpp -c -I./include -o bin/init.o -O3
	g++ src/add.cpp -c -I./include -o bin/add.o -O3
	g++ src/subtract.cpp -c -I./include -o bin/subtract.o -O3
	g++ src/multiply.cpp -c -I./include -o bin/multiply.o -O3
	g++ src/basic.cpp -c -I./include -o bin/basic.o -O3
	g++ src/io.cpp -c -I./include -o bin/io.o -O3
	g++ src/compare.cpp -c -I./include -o bin/compare.o -O3
	g++ src/devide.cpp -c -I./include -o bin/devide.o -O3
	g++ src/math_old.cpp -c -I./include -o bin/math_old.o -O3
	g++ src/arctan.cpp -c -I./include -o bin/arctan.o -O3
	g++ src/string.cpp -c -I./include -o bin/string.o -O3
	ar -cvq bin/libmpix.a bin/*.o 
	rm bin/*.o

clean:
	touch bin/temp
	rm -rf bin/*

test:
	g++ test.cpp -L./bin -lmpix -o bin/test -g2 -pg
