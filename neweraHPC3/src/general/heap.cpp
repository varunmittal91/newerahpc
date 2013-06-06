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

#include <string.h>
#include <unistd.h>
#ifdef __linux__
#include <malloc.h>
#endif

#include <include/heap.h>
#include <include/alloc.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC
{
   void page_empty_data(mem_page_t *page)
   {
      void *_address = (char *)page + sizeof(mem_page_t);
      memset(_address, 0, page_get_size(page));
   }   
   
   void page_merge(mem_page_t *parent, mem_page_t *victim)
   {
      mem_page_t  *tmp  = victim->next;
      nhpc_size_t  size = sizeof(mem_page_t) + page_get_size(victim) + page_get_size(parent);
      
      parent->next = tmp;
      if(tmp)
	 tmp->prev = parent;
      
      page_set_size(parent, size);
   }
   
   Heap::Heap(nhpc_size_t default_size)
   {
      _default_size = default_size;
      _frame_size   = _default_size - (sizeof(mem_frame_t) + sizeof(mem_page_t));
      
      thread_mutex_init(&_frame_mutex);
      
      _frame_root = NULL;
      _create_frame();
      
      pthread_create(&maintainer_thread, NULL, (void* (*)(void*))maintainer, this);
   }
   
   Heap::~Heap()
   {
      mem_frame_t *frame = _frame_root;
      mem_frame_t *next_frame;
      while(frame)
      {
	 next_frame = frame->frame_next;
	 free(frame);
	 frame = next_frame;
      }      
   } 
   
   void Heap::print_table()
   {
      LOG_DEBUG("Memory block status:");
      
      mem_frame_t   *frame;
      mem_page_t    *page;
      int            pageno = 0;
      int            frameno = 0;
      nhpc_status_t  nrv;
      
      frame = _frame_root;
      
      while(frame)
      {
	 nrv = thread_mutex_trylock(&(frame->mutex), NHPC_THREAD_LOCK_READ);
	 if(nrv != NHPC_SUCCESS)
	 {
	    frame = frame->frame_next;
	    continue;
	 }
	 
	 LOG_DEBUG("\t " << frameno << " Frame Address:" << frame);
	 
	 page = frame->page_first;
	 while(page)
	 {
	    LOG_DEBUG("\t\t " << pageno << " Address:" << page << " Prev Address:" << page->prev << " Next Address:" << page->next 
		      << " Size:" << page_get_size(page) << " Status:" << page_status(page));
	    page = page->next;
	    pageno++;	    
	 }
	 if(frame->page_recovered)
	    LOG_DEBUG("\t\t\tRecovered page" << frame->page_recovered << " Size:" << page_get_size(frame->page_recovered));
	 thread_mutex_unlock(&(frame->mutex), NHPC_THREAD_LOCK_READ);
	 
	 frame = frame->frame_next;
	 frameno++;
      }
   }
   
   void Heap::clean_table()
   {
      mem_frame_t    *frame = _frame_root;
      mem_page_t     *page;
      mem_page_t     *page_next;
      mem_page_t    **page_recovered;
      bool            move_next;
      nhpc_status_t   nrv;
      
      while(frame)
      {
	 nrv = thread_mutex_trylock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
	 if(nrv != NHPC_SUCCESS)
	 {
	    frame = frame->frame_next;
	    continue;
	 }
	 
	 page_recovered = &(frame->page_recovered);
	 page           = frame->page_first;
	 while(page)
	 {
	    move_next = true;
	    
	    page_next = page->next;
	    if(!page_next)
	       break;
	    
	    if(page_is_empty(page) && page_is_empty(page_next))
	    {
	       move_next = false;
	       page_merge(page, page_next);
	    }
	    
	    if((page != (frame->page_first)) && page_is_empty(page))
	    {
	       if(!(*page_recovered) || (page_get_size(*page_recovered) < page_get_size(page)))
		  (*page_recovered) = page;
	    }
	    
	    if(move_next)
	       page = page->next;
	 }
	 
	 thread_mutex_unlock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
	 
	 frame = frame->frame_next;
      }
   }
   
   void Heap::maintainer(Heap *HeapObject)
   {
      while(1)
      {
	 (*HeapObject).clean_table();
	 if(log_is_debug_enabled())
	 {
	    (*HeapObject).print_table();
	 }
	 sleep(2);
      }
   }
   
   mem_frame_t *Heap::_create_frame()
   {
      mem_frame_t *frame = (mem_frame_t *)malloc(_default_size);
      memset(frame, 0, _default_size);
      
      thread_mutex_init(&(frame->mutex));
      
      frame->page_first     = (mem_page_t *)((char *)frame + sizeof(mem_frame_t));
      nhpc_size_t page_size = _default_size - (sizeof(mem_frame_t) + sizeof(mem_page_t));
      page_set_size((frame->page_first), page_size);
      
      thread_mutex_lock(&_frame_mutex, NHPC_THREAD_LOCK_WRITE);
      
      mem_frame_t **new_frame = &(_frame_root);
      mem_frame_t  *parent    = NULL;
      
      while(*new_frame)
      {
	 parent    = (*new_frame);
	 new_frame = &((*new_frame)->frame_next);
      }
      
      (*new_frame) = frame;
      if(parent)
	 parent->frame_next = frame;
      
      thread_mutex_unlock(&_frame_mutex, NHPC_THREAD_LOCK_WRITE);
      
      return frame;
   }
   
   mem_page_t *Heap::_create_page(mem_page_t *parent, nhpc_size_t size)
   {
      nhpc_size_t parent_size = page_get_size(parent);
      parent_size = parent_size - (size + sizeof(mem_page_t));
      
      mem_page_t *new_page = (mem_page_t *)((char *)parent + sizeof(mem_page_t) + parent_size);
      memset(new_page, 0, (sizeof(mem_page_t) + size));
      page_set_size(new_page, size);
      page_set_size(parent,   parent_size);
      
      mem_page_t *tmp = parent->next;
      if(tmp)
	 new_page->next = tmp;
      new_page->prev = parent;
      parent->next   = new_page;
      
      return new_page;
   }
   
   mem_page_t *Heap::_fetch_page(nhpc_size_t size)
   {
      mem_frame_t *frame = _frame_root;
      
      mem_page_t   *new_page;
      mem_page_t  **page_recovered;
      mem_page_t  **page_first;
      nhpc_size_t   page_recovered_size;
      nhpc_size_t   page_first_size;
      bool          use_recovered;
      nhpc_status_t nrv;
      
   read_frame:
      nrv = thread_mutex_trylock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
      if(nrv != NHPC_SUCCESS)
      {
	 if(frame->frame_next)
	    frame = frame->frame_next;
	 else 
	    frame = _frame_root;
	 goto read_frame;
      }
      
      page_recovered  = &(frame->page_recovered);
      page_first      = &(frame->page_first);
      page_first_size = page_get_size(*page_first);
      new_page        = NULL;
      use_recovered   = false;
      
      if(*page_recovered)
	 page_recovered_size = page_get_size(*page_recovered);
      else 
	 page_recovered_size = 0;
      
      if(page_recovered_size >= size)
      {
	 new_page        = *page_recovered;
	 use_recovered   = true;
      }
      else if(page_is_empty(*page_first) && page_first_size >= size)
	 new_page = *page_first;
      
      if(new_page)
      {
	 if(page_get_size(new_page) > (size + sizeof(mem_page_t)))
	 {
	    _create_page(new_page, size);

	    if(use_recovered)
	       *page_recovered = new_page;
	    
	    new_page = new_page->next;	    
	 }
	 else 
	 {
	    if(use_recovered)
	       *page_recovered = NULL;
	 }  
	 page_set_occupied(new_page);
      }
      thread_mutex_unlock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
      
      if(!new_page)
      {
	 thread_mutex_lock(&_frame_mutex, NHPC_THREAD_LOCK_WRITE);
	 
	 if(frame->frame_next)
	    frame = frame->frame_next;
	 else 
	    frame = _create_frame();
	 
	 thread_mutex_unlock(&_frame_mutex, NHPC_THREAD_LOCK_WRITE);
	 
	 goto read_frame;
      }
      
      return new_page;
   }
   
   void *Heap::allocate(nhpc_size_t size)
   {
      mem_page_t *page    = _fetch_page(size);
      
      if(!page)
	 return NULL;
      (page->address) = (void *)((char *)page + sizeof(mem_page_t));
      
      return (page->address);
   }
   
   void Heap::deallocate(void *address)
   {
      mem_page_t *in_mem_page = (mem_page_t *)((char *)address - sizeof(mem_page_t));
      
      if(page_is_empty(in_mem_page) || !(in_mem_page->address))
      {
	 LOG_ERROR("Block Deallocated");
      }
      
      in_mem_page->address = NULL;
      page_set_empty(in_mem_page);
   }   
}
