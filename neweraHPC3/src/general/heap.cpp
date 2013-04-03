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
   Heap::Heap(nhpc_size_t default_size)
   {
      _default_size = default_size;
      _frame_size   = _default_size - (sizeof(mem_frame_t) + sizeof(mem_page_t));
      
      thread_mutex_init(&frame_mutex);
      
      frame_root = NULL;
      _create_frame();
      
      pthread_create(&maintainer_thread, NULL, (void* (*)(void*))maintainer, this);
   }
   
   Heap::~Heap()
   {
      mem_frame_t *frame = frame_root;
      mem_frame_t *next_frame;
      while(frame)
      {
	 next_frame = frame->frame_next;
	 free(frame);
	 frame = next_frame;
      }      
   }
   
   mem_frame_t *Heap::_create_frame()
   {
      mem_frame_t *new_frame = (mem_frame_t *)malloc(_default_size);
      memset(new_frame, 0, _default_size);
      new_frame->frame_next = NULL;
      new_frame->page_first = (mem_page_t *)((char *)new_frame + sizeof(mem_frame_t));
      page_set_size(new_frame->page_first, _frame_size);
      thread_mutex_init(&(new_frame->mutex));
      
      if(!frame_root)
	 frame_root = new_frame;
      else 
      {
	 mem_frame_t *tmp_frame = frame_root;
	 while(tmp_frame->frame_next)
	 {
	    tmp_frame = tmp_frame->frame_next;
	 }
	 tmp_frame->frame_next = new_frame;
      }
      frame_current = new_frame;
      return new_frame;
   }
   
   mem_page_t *Heap::_create_page(mem_page_t *parent, nhpc_size_t child_size)
   {
      nhpc_size_t parent_size = page_get_size(parent);
      
      mem_page_t *child_page = (mem_page_t *)((char *)parent  + sizeof(mem_page_t) + parent_size - child_size);
      memset(child_page, 0, child_size);
      
      parent_size -= child_size;
      page_set_size(parent, parent_size);      
      page_set_size(child_page, (child_size - sizeof(mem_page_t)));

      child_page->prev = parent;
      child_page->next = parent->next;
      parent->next = child_page;
      
      if(child_page->next)
	 child_page->next->prev = child_page;
      
      return child_page;
   }
   
   mem_page_t *Heap::_fetch_page(nhpc_size_t size)
   {
      mem_frame_t *frame = frame_root;
      
      mem_page_t   *recovered;
      mem_page_t   *page;
      mem_page_t   *new_page;
      mem_page_t  **recovered_page;
      nhpc_size_t   page_size;
      
      size += sizeof(mem_page_t);
      
   read_frame:
      thread_mutex_lock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
      page           = frame->page_first;
      page_size      = page_get_size(page);
      recovered_page = &(frame->page_recovered);
      new_page       = NULL;
      recovered      = frame->page_recovered;
      
      if(*recovered_page && page_is_empty(*recovered_page) && page_get_size(*recovered_page) >= size)
      {
	 if(page_get_size(*recovered_page) > (sizeof(mem_page_t) + size))
	 {
	    new_page = _create_page(*recovered_page, size);
	 }
	 else 
	 {
	    new_page = (*recovered_page);
	    frame->page_recovered = NULL;
	    _clean_pages(frame);
	 }
      }
      else if(page_is_empty(page) && page_size >= size)
      {
	 if(page_size > (size + sizeof(mem_page_t)))
	 {
	    _create_page(page, size);
	    new_page = page->next;
	 }
	 else 
	    new_page = page;	    
      }
      
      if(new_page)
      {
	 page_set_occupied(new_page);  
	 thread_mutex_unlock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
      }
      else 
      {
	 thread_mutex_unlock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);

	 thread_mutex_lock(&frame_mutex, NHPC_THREAD_LOCK_WRITE);
	 if(frame->frame_next)
	    frame = frame->frame_next;
	 else 
	    frame = _create_frame();
	 thread_mutex_unlock(&frame_mutex, NHPC_THREAD_LOCK_WRITE);
      }
      
      if(!new_page)
	 goto read_frame;
      
      return new_page;
   }
   
   void Heap::_clean_pages(mem_frame_t *frame)
   {
      mem_page_t  *in_mem_page = frame->page_first;;
      nhpc_size_t  in_mem_page_size;
      mem_page_t  *next_mem_page;
      mem_page_t **page_recovered = &(frame->page_recovered);
      bool         move_next;
      
      while(in_mem_page)
      {
	 move_next = false;
	 next_mem_page = in_mem_page->next;

	 if(page_is_empty(in_mem_page))
	 {
	    if(next_mem_page && page_is_empty(next_mem_page))
	    {
	       page_merge(in_mem_page, next_mem_page);	       
	       memset(next_mem_page, 0, sizeof(mem_page_t));
	    }
	    else 
	       move_next = true;
	    
	    if(in_mem_page != (frame->page_first))
	    {
	       if(!(*page_recovered) || (page_get_size(in_mem_page) > page_get_size(*page_recovered)))
	       {
		  (*page_recovered) = in_mem_page;
	       }
	    }
	 }
	 else
	    move_next = true;
	    
	 if(move_next)
	    in_mem_page = in_mem_page->next;
      }    
      
      if(*page_recovered && (page_get_size(*page_recovered) > _default_size))
	 *page_recovered = NULL;
   }
   
   void *Heap::allocate(nhpc_size_t size)
   {
      mem_page_t *page = _fetch_page(size);
      void       *address = NULL;
      
      if(page)
      {
	 address = (void *)((char *)page + sizeof(mem_page_t));
	 memset(address, 0, size);
      }
      page->address = address;
      
      return address;      
   }
   
   void Heap::deallocate(void *_address)
   {
      mem_page_t *in_mem_page   = (mem_page_t *)((char *)_address - sizeof(mem_page_t));
      if(page_is_empty(in_mem_page))
      {
	 LOG_ERROR("HEAP: Resource already deallocated");
	 return;
      }
      
      if((in_mem_page->size) > _default_size || _address != in_mem_page->address)
      {
	 LOG_ERROR("Memory block already deallocated");
      }
      else 
      {
	 page_set_empty(in_mem_page);  
	 in_mem_page->address = NULL;
      }
   }
   
   void Heap::maintain_frame()
   {
      LOG_DEBUG("Memory block status:");

      mem_frame_t *frame;
      mem_page_t  *page;
      int          pageno = 0;
      int          frameno = 0;

      thread_mutex_lock(&frame_mutex, NHPC_THREAD_LOCK_READ);
      frame = frame_root;

      while(frame)
      {
	 thread_mutex_lock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
	 _clean_pages(frame);
	 
	 LOG_DEBUG("\t " << frameno << " Frame Address:" << frame);

	 page = frame->page_first;
	 while(page)
	 {
	    LOG_DEBUG("\t\t " << pageno << " Address:" << page << " Prev Address:" << page->prev << " Next Address:" << page->next << 
		      " Size:" << page_get_size(page) << " Status:" << page_status(page));
	    page = page->next;
	    pageno++;	    
	 }
	 thread_mutex_unlock(&(frame->mutex), NHPC_THREAD_LOCK_WRITE);
	 
	 frame = frame->frame_next;
	 frameno++;
      }
      thread_mutex_unlock(&frame_mutex, NHPC_THREAD_LOCK_READ);
   }
   
   void Heap::maintainer(Heap *HeapObject)
   {
      while(1)
      {
	 HeapObject->maintain_frame();
	 sleep(1);
      }
   }
   
   void page_empty_data(mem_page_t *page)
   {
      void *_address = (char *)page + sizeof(mem_page_t);
      memset(_address, 0, page_get_size(page));
   }   
   
   void page_merge(mem_page_t *parent, mem_page_t *victim)
   {
      mem_page_t  *tmp  = victim->next;
      nhpc_size_t  size = sizeof(mem_page_t) + page_get_size(victim) + page_get_size(parent);
      page_set_size(parent, size);
      
      parent->next = tmp;
      if(tmp)
	 tmp->prev = parent;
   }
}
