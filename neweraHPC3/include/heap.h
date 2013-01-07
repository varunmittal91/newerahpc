/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com>
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

#ifndef _NHPC_HEAP_H_
#define _NHPC_HEAP_H_

#include "list.h"
#include "neweraHPC.h"

#define heap_set_end_page

#define PAGE_EMPTY    0
#define PAGE_OCCUPIED 1

#define page_status(p)        ((p)->size & 1)
#define page_get_size(p)      ((p)->size >> 1)
#define page_set_size(p, s)   ((p)->size = ((p)->size & 1) | (s << 1))
#define page_set_empty(p)     ((p)->size &= ~1)
#define page_set_occupied(p)  ((p)->size |= 1)
#define page_is_empty(p)      (!page_status(p))
#define page_is_occupied(p)   (page_status(p))

namespace neweraHPC
{
   class GarbageCollector
   {
   private:
      struct mem_page_t
      {
	 nhpc_size_t  size;
	 mem_page_t  *next;
	 mem_page_t  *prev;
      };
      struct mem_frame_t
      {
	 mem_frame_t      *frame_next;
	 mem_page_t       *page_first;
	 pthread_mutex_t   mutex;
      };
      
      nhpc_size_t size;
      
      pthread_mutex_t  mutex;
      pthread_t        maintinance_thread;
      
      mem_frame_t *frame_root;
      mem_frame_t *frame_current;
      nhpc_size_t  frame_size;
      
      mem_frame_t *frame_create();
      mem_frame_t *frame_get(nhpc_size_t);
      mem_page_t  *page_get(nhpc_size_t);
      
      void clean_pages();
      static void *maintain_thread(GarbageCollector *object);
   
      bool heap_ready;
   public:
       GarbageCollector(unsigned int _size = 10485760);
      ~GarbageCollector();
      
      void *allocate(size_t size);
      void  deallocate(void *_address);
      
      void add_elem(int number);
      
      void status();
   };
};

#endif