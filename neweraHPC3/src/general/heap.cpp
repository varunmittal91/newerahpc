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
   GarbageCollector::GarbageCollector(unsigned int _size)
   {      
      pthread_mutex_init(&mutex, NULL);
      pthread_mutex_lock(&mutex);
      
      size         = _size;    
            
      frame_size    = size - (sizeof(mem_frame_t) + sizeof(mem_page_t));
      frame_root    = (mem_frame_t *)malloc(size);
      frame_current = frame_root;
      
      frame_root->page_first  = (mem_page_t *)((char *)frame_root + sizeof(mem_frame_t));
      page_set_size(frame_root->page_first, frame_size);
      pthread_mutex_init(&(frame_root->mutex), NULL);
      
      pthread_create(&maintinance_thread, NULL, (void* (*)(void*))maintain_thread, this);
      
      pthread_mutex_unlock(&mutex);
   }

   GarbageCollector::~GarbageCollector()
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
      
   void GarbageCollector::status()
   {
      LOG_DEBUG("Memory block status:");

      mem_frame_t *frame = frame_root;
      mem_page_t  *page;
      int          pageno = 0;
      int          frameno = 0;
      
      while(frame)
      {
	 page = frame->page_first;
	 
	 LOG_DEBUG("\t " << frameno << " Frame Address:" << frame);
	 while(page)
	 {
	    LOG_DEBUG("\t\t " << pageno << " Address:" << page << " Prev Address:" << page->prev << " Next Address:" << page->next << 
		      " Size:" << page_get_size(page) << " Status:" << page_status(page));
	    page = page->next;
	    pageno++;
	 }
	 
	 frame = frame->frame_next;
	 
	 frameno++;
      }
   }
   
   void GarbageCollector::clean_pages()
   {
      mem_frame_t *frame = frame_root;
      
      while(frame)
      {
	 pthread_mutex_lock(&(frame->mutex));
	 
	 mem_page_t *in_mem_page = frame->page_first;;
	 mem_page_t *next_mem_page;
	 mem_page_t **page_recovered = &(frame->page_recovered);
	 mem_page_t *check_recovered = NULL;

	 while(in_mem_page)
	 {
	    next_mem_page = in_mem_page->next;
	 
	    if(page_is_empty(in_mem_page))
	       check_recovered = in_mem_page;
	    
	    if(page_is_empty(in_mem_page) && next_mem_page && page_is_empty(next_mem_page))
	    {
	       nhpc_size_t in_mem_page_size = page_get_size(in_mem_page) + sizeof(mem_page_t) + page_get_size(next_mem_page);
	       page_set_size(in_mem_page, in_mem_page_size);
	       in_mem_page->next =  next_mem_page->next;
	       memset((next_mem_page), 0, (sizeof(mem_page_t) + page_get_size(next_mem_page)));
	    
	       if(next_mem_page->next)
		  next_mem_page->next->prev = in_mem_page;
	    }
	    else 
	       in_mem_page = in_mem_page->next;
	    
	    if(check_recovered && check_recovered != frame->page_first)
	    {
	       if(!*page_recovered)
		  *page_recovered = check_recovered;
	       else if(!page_is_empty(*page_recovered))
		  *page_recovered = check_recovered;
	       else if(page_get_size(check_recovered) > page_get_size(*page_recovered)) 
		  *page_recovered = check_recovered;
	       
	       check_recovered = NULL;
	    }
	 }

	 pthread_mutex_unlock(&(frame->mutex));
	 frame = frame->frame_next;
      }
   }
   
   void *GarbageCollector::maintain_thread(GarbageCollector *object)
   {
      while(1)
      {
	 (*object).clean_pages();
	 (*object).status();
	 sleep(1);
      }
   }
   
   GarbageCollector::mem_frame_t * GarbageCollector::frame_create()
   {
      mem_frame_t *new_frame = (mem_frame_t *)malloc(size);
      memset(new_frame, 0, size);
      new_frame->frame_next = NULL;
      new_frame->page_first = (mem_page_t *)((char *)new_frame + sizeof(mem_frame_t));
      page_set_size(new_frame->page_first, frame_size);
      pthread_mutex_init(&(new_frame->mutex), NULL);  
      
      mem_frame_t *frame = frame_root;
      while(frame->frame_next)
	 frame = frame->frame_next;
      frame->frame_next = new_frame;
      
      frame_current = new_frame;
      
      return new_frame;
   }
   
   void GarbageCollector::page_create(mem_page_t *parent, nhpc_size_t child_size)
   {
      nhpc_size_t parent_size = page_get_size(parent);

      mem_page_t *child_page = (mem_page_t *)((char *)parent  + sizeof(mem_page_t) + parent_size - child_size);
      memset(child_page, 0, sizeof(mem_page_t));
      
      parent_size -= child_size;
      page_set_size(parent, parent_size);
      
      child_page->prev = parent;
      child_page->next = parent->next;
      page_set_size(child_page, (child_size - sizeof(mem_page_t)));
      parent->next = child_page;
      
      if(child_page->next)
	 child_page->next->prev = child_page;
   }
   
   GarbageCollector::mem_page_t *GarbageCollector::page_get(nhpc_size_t size)
   {
      mem_frame_t *frame;
      mem_page_t  *page;
      mem_page_t  *new_page = NULL;
      mem_page_t  **page_recovered;
      nhpc_size_t  page_size;
      
      size += sizeof(mem_page_t);
      
      pthread_mutex_lock(&mutex);
      frame = frame_root;
      while(frame)
      {
	 pthread_mutex_lock(&(frame->mutex));
	 page = frame->page_first;
	 page_size = page_get_size(page);
	 page_recovered = &(frame->page_recovered);
	 
	 if(*page_recovered && page_is_empty(*page_recovered) && page_get_size(*page_recovered) >= size)
	 {
	    if(page_get_size(*page_recovered) > (size + sizeof(mem_page_t)))
	    {
	       page_create(*page_recovered, size);
	       new_page = (*page_recovered)->next;
	    }
	    else 
	       new_page = *page_recovered;
	 }
	 else if(page_is_empty(page) && page_size >= size)
	 {
	    if(page_size > (size + sizeof(mem_page_t)))
	    {
	       page_create(page, size);
	       new_page = page->next;
	    }
	    else 
	       new_page = page;
	 }
	 pthread_mutex_unlock(&(frame->mutex));
	 if(new_page)
	 {
	    page_set_occupied(new_page);  
	    break;
	 }
	 else 
	 {
	    if(frame->frame_next)
	       frame = frame->frame_next;
	    else 
	       frame = frame_create();
	 }
      }
      pthread_mutex_unlock(&mutex);
      
      return new_page;
   }
   
   void *GarbageCollector::allocate(size_t size)
   {
      mem_page_t *page = page_get(size);
      void       *address = NULL;
      
      if(page)
      {
	 address = (void *)((char *)page + sizeof(mem_page_t));
	 memset(address, 0, size);
      }
      
      return address;
   }
   
   void GarbageCollector::deallocate(void *_address)
   {
      mem_page_t *in_mem_page   = (mem_page_t *)((char *)_address - sizeof(mem_page_t));
      if(page_is_empty(in_mem_page))
      {
	 LOG_ERROR("HEAP: Resource already deallocated");
	 return;
      }
      
      page_set_empty(in_mem_page);
      
      LOG_DEBUG("HEAP: \t\tDelete requested on page size:" << in_mem_page->size);
   }
}
