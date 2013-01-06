/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v2
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 2 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/alloc.h>
#include <include/heap.h>
#include <include/error.h>

using namespace std;

#ifdef ENABLE_GARBAGE_COLLECTOR
namespace neweraHPC
{
   GarbageCollector garbagecollector;
};

void *operator new(std::size_t size) throw (std::bad_alloc)
{
   void *new_p = garbagecollector.allocate(size);
   memset(new_p, 0, size);

   return new_p;
}

void *operator new[](std::size_t size) throw (std::bad_alloc)
{
   void *new_p = operator new(size);
   return new_p;
}

void operator delete(void *ptr) throw ()
{
   garbagecollector.deallocate(ptr);
}

void operator delete[](void *ptr) throw ()
{
   operator delete(ptr);
}   
#endif
