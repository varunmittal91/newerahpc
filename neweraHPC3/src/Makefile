# src/Makefile

include ../Makefile.inc

SUBDIRS = general network

OBJECTS := $(SUBDIRS:=/*.o)

$(OBJLIB): libs dynamic_lib

libs:
	for subdir in $(SUBDIRS); do \
	cd $$subdir; $(MAKE) $(MKFLAGS); \
	cd ..; \
	done; 
	
dynamic_lib:
	$(CPPC) $(DYNAMIC_FLAGS) $(OBJECTS)
	
clean:
	for subdir in $(SUBDIRS); do \
	cd $$subdir; $(MAKE) clean $(MKFLAGS); \
	echo $$subdir; \
	cd ..; \
	done;
	rm -f $(OBJLIB)