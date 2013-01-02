#ifndef _NHPC_HEAP_H_
#define _NHPC_HEAP_H_

#include "list.h"
#include "neweraHPC.h"

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
	 void        *address;
      };
      
      unsigned int size;
      unsigned int size_allocated;
      
      char *start_address;
      char *end_address;
      void *memory_space;
      
      mem_page_t      *mem_page;
      mem_page_t      *last_mem_page;
      pthread_mutex_t  mutex;
      pthread_t        maintinance_thread;
      
      void clean_pages();
      static void *maintain_thread(GarbageCollector *object);
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