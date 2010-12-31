CC=g++
INCLUDE_PATH := -I../../include
EXTRA_CFLAGS_SERVER := -Dnewerahpc -g $(INCLUDE_PATH) -Ddebug -pg
SOURCES=file.cpp log.cpp string.cpp queue.cpp tree.cpp rbtree.cpp
SOURCES_TEST=test.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS_TEST=$(SOURCES_TEST:test.cpp=test)
EXECUTABLE=grid_all.a

all: $(SOURCES) $(SOURCES_TEST) grid_all.a 

$(EXECUTABLE): $(OBJECTS) $(OBJECTS_TEST)

.cpp.o:
	$(CC) $< -o $@ $(INCLUDE_PATH) -c $(EXTRA_CFLAGS_SERVER)
.cpp:
	$(CC) *.o $< -o $@

clean:
	rm *.o
	rm test

