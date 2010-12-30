CC=g++
INCLUDE_PATH := -I../../include
EXTRA_CFLAGS_SERVER := -Dnewerahpc -g $(INCLUDE_PATH) -Ddebug -pg
SOURCES= memory.cpp alloc.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=grid_all.a


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)

.cpp.o:
	$(CC) $< -o $@ $(INCLUDE_PATH) -c $(EXTRA_CFLAGS_SERVER)

clean:
	rm *.o

