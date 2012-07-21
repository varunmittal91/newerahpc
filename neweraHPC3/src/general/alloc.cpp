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

using namespace std;

#ifdef ENABLE_GARBAGE_COLLECTOR
void *operator new(std::size_t size)
{
   void *new_p = NULL;
   
   if(garbage_collector_ready)
   {
      new_p = nhpc_allocate_str(size);
   }
   else 
   {
      LOG_ERROR("from outside garbage collector");
      
      size_t _size = size + sizeof(size_t);
      
      new_p = alloc<char>(_size);
      memset(new_p, 0, _size);
      
      size_t *str_len = (size_t *)new_p;
      *str_len = size;
      
      new_p = (char *)new_p + sizeof(size_t);
   }
   
   if(!new_p)
   {
      LOG_ERROR("Allocation error, errno: " << errno);
      exit(0);
   }
   
   return new_p;
}

void *operator new[](std::size_t size)
{
   void *new_p = operator new(size);
   return new_p;
}

void operator delete(void *ptr)
{
   if(garbage_collector_ready)
      nhpc_deallocate_str((char *)ptr);
}

void operator delete[](void *ptr)
{
   operator delete(ptr);
}   
#endif
