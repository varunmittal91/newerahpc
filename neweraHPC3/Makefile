include ./Makefile.inc

DIRS   = src tests
EXE    = server

all: $(EXE)
	
$(EXE): $(OBJLIB)	
	$(CPPC) server.cpp -o $(EXE) $(LIBS) src/$(OBJLIB) $(PRJFLAGS)

$(OBJLIB):
	cd src; $(MAKE) $(MKFLAGS)
	
tests: $(OBJLIB)
	cd tests; $(MAKE) $(MKFLAGS)
	
clean:
	rm -f $(EXE)
	cd src; $(MAKE) clean $(MKFLAGS)
	

