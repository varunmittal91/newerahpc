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

#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <errno.h>
#ifdef linux
#include <malloc.h>
#endif
#include <stdlib.h>

#include "error.h"
#include "strings_pool.h"

using namespace neweraHPC;

void *operator new(std::size_t size)
{
   LOG_INFO("using overloaded new");
   
   void *new_p = NULL;

   if(garbage_collector_ready)
      new_p = nhpc_allocate_str(size);
   else 
   {
      new_p = malloc(size);
      memset(new_p, 0, size);
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
   else 
      free(ptr);
}

void operator delete[](void *ptr)
{
   operator delete(ptr);
}

#endif
