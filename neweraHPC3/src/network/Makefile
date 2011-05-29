include ../../Makefile.inc

SRC = socket.cpp
OBJ = $(SRC:.cpp=.o)

INCLUDE_DIR = -I../../

.cpp.o:
	$(CPPC) -o $@ $? $(PRJFLAGS) $(INCLUDE_DIR) -c

all: $(SRS) $(OBJ)

clean:
	rm -f $(OBJ)
	
