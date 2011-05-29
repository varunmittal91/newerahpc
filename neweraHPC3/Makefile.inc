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
	OBJLIB		  = libneweraHPC.so.$(MAJOR_RELEASE).$(MINOR_RELEASE)
	DYNAMIC_FLAGS = -shared -Wl,-soname,libneweraHPC.so.$(MAJOR_RELEASE) -o libneweraHPC.so.$(MAJOR_RELEASE).$(MINOR_RELEASE)
endif
ifeq ($(UNAME),Darwin)
	OBJLIB        = libneweraHPC.$(MAJOR_RELEASE).dylib
	DYNAMIC_FLAGS = -dynamiclib -o libneweraHPC.$(MAJOR_RELEASE).dylib
endif

