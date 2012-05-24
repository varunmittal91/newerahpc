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

#include "error.h"

using namespace neweraHPC;

void *operator new(std::size_t size)
{
   void *new_p = NULL;
   new_p = malloc(size);
   if(!new_p)
   {
      LOG_ERROR("Allocation error, errno: " << errno);
      exit(0);
   }

   memset(new_p, 0, size);
   
   return new_p;
}

void *operator new[](std::size_t size)
{
   void *new_p = operator new(size);
   return new_p;
}

namespace neweraHPC
{
   template <class T>
   T *alloc(int n_items)
   {
      T *new_p = (T *)malloc(sizeof(T) * n_items);
      if(new_p == NULL)
      {
	 LOG_ERROR("Unsafe Allocation, errno: " << errno);
	 exit(0);
      }
   }   
   
   /*
   template <class T>
   void *alloc(int n_items)
   {
      T *new_p = malloc(sizeof(T) * n_items);
      if(new_p == NULL)
      {
	 LOG_ERROR("Unsafe Allocation, errno: " << errno);
	 exit(0);
      }
   }
   */
   
   /*
   void *alloc(int size)
   {
      
   }
    */
};

#endif