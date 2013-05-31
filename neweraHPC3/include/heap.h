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

#include <pthread.h>

#include "constants.h"
#include "thread.h"

#define heap_set_end_page

#define PAGE_EMPTY    0
#define PAGE_OCCUPIED 1

#define MIN_PAGE_SIZE 64  // Minimum page size is set to 16bytes.

#define page_status(p)        ((p)->size & 1)
#define page_get_size(p)      ((p)->size >> 1)
#define page_set_size(p, s)   do { (p->size) &= 1; (p->size) |= (s << 1); }while(false) 
#define page_set_empty(p)     ((p)->size &= ~1)
#define page_set_occupied(p)  ((p)->size |= 1)
#define page_is_empty(p)      (!page_status(p))
#define page_is_occupied(p)   (page_status(p))

namespace neweraHPC
{
   struct mem_page_t
   {
      nhpc_size_t   size;
      mem_page_t   *next;
      mem_page_t   *prev;
      void         *address;
   };
   struct mem_frame_t
   {
      mem_frame_t  *frame_next;
      mem_page_t   *page_first;
      mem_page_t   *page_recovered;
      nhpc_mutex_t  mutex;
   };
   
   void page_empty_data(mem_page_t *page);
   void page_merge(mem_page_t *parent, mem_page_t *victim);
   
   class Heap
   {
   private:
      nhpc_size_t  _default_size;
      nhpc_size_t  _frame_size;
      mem_frame_t *_frame_root;
      mem_frame_t *_frame_current;
      
      nhpc_mutex_t _frame_mutex;
      pthread_t	  maintainer_thread;
      
      mem_frame_t *_create_frame();
      mem_page_t  *_create_page(mem_page_t *parent, nhpc_size_t child_size);
      mem_page_t  *_fetch_page(nhpc_size_t size);
      
      void _clean_pages(mem_frame_t *frame);
      
      static void maintainer(Heap *HeapObject);
      void        maintain_frame();
      
   public:
      Heap(nhpc_size_t default_size = 10485760);
      ~Heap();
      
      void *allocate(nhpc_size_t _size);
      void  deallocate(void *_address);
      
      void print_table();
      void clean_table();
   };
};

#endif