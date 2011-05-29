MAJOR_RELEASE = 2
MINOR_RELEASE = 90

UNAME := $(shell uname)

CPPC   = g++
CC     = gcc
LIBS   = 

ECHO   = echo

MKFLAGS = 

PRJFLAGS = -g 

ifeq ($(UNAME),Linux)
	OBJLIB		  = libneweraHPC.so.$(MAJOR_RELEASE)
	DYNAMIC_FLAGS = -shared -Wl,-soname,libneweraHPC.so.$(MAJOR_RELEASE) -o libneweraHPC.so.$(MAJOR_RELEASE).$(MINOR_RELEASE)
endif
ifeq ($(UNAME),Darwin)
	OBJLIB        = libneweraHPC.$(MAJOR_RELEASE).dylib
	DYNAMIC_FLAGS = -dynamiclib -o libneweraHPC.$(MAJOR_RELEASE).dylib
endif

