#include <iostream>

#include <include/alloc.h>

using namespace std;

namespace neweraHPC
{
   GarbageCollector::GarbageCollector(unsigned int _size)
   {      
      pthread_mutex_init(&mutex, NULL);
      pthread_mutex_lock(&mutex);
      
      size         = _size;    
      memory_space = malloc(_size);
      end_address  = (char *)memory_space + size;
      memset(memory_space, 0, _size);
      
      mem_page	    = (mem_page_t *)memory_space;
      last_mem_page = mem_page;
      memset(mem_page, 0, sizeof(mem_page_t));
      mem_page->size = size - sizeof(mem_page_t);
      size_allocated = sizeof(mem_page_t);

      pthread_create(&maintinance_thread, NULL, (void* (*)(void*))maintain_thread, this);
      
      pthread_mutex_unlock(&mutex);
   }

   GarbageCollector::~GarbageCollector()
   {
      free(memory_space);
   }
   
   void GarbageCollector::status()
   {
      LOG_DEBUG("Memory block status:");
      mem_page_t *tmp_mem_page = mem_page;
      int pageno = 0;
      while(tmp_mem_page)
      {
	 LOG_DEBUG("\t" << pageno << " Address:" << tmp_mem_page << " Prev address:" << tmp_mem_page->prev 
		   << " Next address:" << tmp_mem_page->next << " Size:" << tmp_mem_page->size 
		   << " Status:" << tmp_mem_page->address);
	 tmp_mem_page = tmp_mem_page->next;
	 pageno++;
      }
   }
   
   void GarbageCollector::clean_pages()
   {
      mem_page_t *in_mem_page = mem_page;
      mem_page_t *next_mem_page;

      while(in_mem_page)
      {
	 next_mem_page = in_mem_page->next;
	 
	 if(!(in_mem_page->address) && next_mem_page && !(next_mem_page->address))
	 {
	    in_mem_page->size += (sizeof(mem_page_t) + next_mem_page->size);
	    in_mem_page->next =  next_mem_page->next;
	    memset((next_mem_page), 0, (sizeof(mem_page_t) + next_mem_page->size));
	    
	    if(next_mem_page->next)
	       next_mem_page->next->prev = in_mem_page;
	 }
	 else 
	    in_mem_page = in_mem_page->next;
      }
   }
   
   void *GarbageCollector::maintain_thread(GarbageCollector *object)
   {
      while(1)
      {
	 pthread_mutex_lock(&(object->mutex));
	 (*object).clean_pages();
	 (*object).status();
	 pthread_mutex_unlock(&(object->mutex));
	 sleep(1);
      }
   }
   
   void *GarbageCollector::allocate(size_t size)
   {
      size += sizeof(mem_page_t);
      
      mem_page_t *tmp = mem_page, *new_page = NULL;
      
      pthread_mutex_lock(&mutex);
      while(tmp)
      {
	 if(!(tmp->address) && tmp->size >= size)
	 {
	    if(tmp->size > (size + sizeof(mem_page_t)))
	    {
	       new_page = (mem_page_t *)((char *)tmp + sizeof(mem_page_t) + tmp->size - size);
	       memset(new_page, 0, sizeof(mem_page_t));
	       
	       new_page->prev = tmp;
	       new_page->next = tmp->next;
	       new_page->size = size - sizeof(mem_page_t);
	       tmp->next = new_page;
	       tmp->size -= size;
	       
	       if(new_page->next)
		  new_page->next->prev = new_page;
	    }
	    else 
	    {
	       new_page = tmp;
	    }
	    
	    new_page->address = (char *)new_page + sizeof(mem_page_t);
	    break;
	 }
	 
	 tmp = tmp->next;
      }
      pthread_mutex_unlock(&mutex);

      if(new_page)
      {
	 memset((new_page->address), 0, new_page->size);
	 return new_page->address;
      }
      else 
      {
	 LOG_ERROR("HEAP: Memory Exhausted");
	 return NULL;
      }
   }
   
   void GarbageCollector::deallocate(void *_address)
   {
      if(_address < memory_space || _address > end_address)
      {
	 LOG_ERROR("HEAP: Out of range address requested");
	 exit(1);
      }
      
      mem_page_t *in_mem_page   = (mem_page_t *)((char *)_address - sizeof(mem_page_t));
      if(!(in_mem_page->address))
      {
	 LOG_ERROR("HEAP: Resource already deallocated");
	 return;
      }
      
      LOG_DEBUG("HEAP: \t\tDelete requested on page size:" << in_mem_page->size);

      in_mem_page->address = NULL;
   }
}
