#
#	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
#	NeweraHPC program is distributed under the terms of the GNU General Public License v2
#
#	This file is part of NeweraHPC.
#
#	NeweraHPC is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation version 2 of the License.
#
#	NeweraHPC is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
#

include ./Makefile.inc

SRC = server.cpp

DIRS   = src tests
EXE    = server

all: $(EXE)
	
$(EXE): $(OBJLIB)	
	$(CPPC) $(SRC) -o $(EXE) $(LIBS) -Lsrc -lneweraHPC $(PRJFLAGS)

$(OBJLIB):
	cd src; $(MAKE) $(MKFLAGS)
	
tests: $(OBJLIB)
	cd tests; $(MAKE) $(MKFLAGS)
	
clean:
	rm -f $(EXE)
	cd src; $(MAKE) clean $(MKFLAGS)
	

