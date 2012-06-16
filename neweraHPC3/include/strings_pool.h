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

#ifndef _STRINGS_POOL_H_
#define _STRINGS_POOL_H_

#include "rbtree.h"
#include "thread.h"

#define MAX_STRING_COUNT 1000

namespace neweraHPC
{   
   extern bool garbage_collector_ready;
   
   class strings_pool_t
   {
   private:
      rbtree_t *strings_free;
      rbtree_t *strings_allocated;
      nhpc_mutex_t *mutex_free;
      nhpc_mutex_t *mutex_allocated;
      
      int allocated_count;
      int free_count;
      
      struct pool_string_t
      {
	 char *string;
	 nhpc_size_t len;
      };
      
      void clean_strings();
   public:
      strings_pool_t();
      ~strings_pool_t();
      
      char *search_string(nhpc_size_t str_len);
      void free_string(char *address);
   };
   
   char *nhpc_allocate_str(nhpc_size_t str_len);
   void nhpc_deallocate_str(char *address);
   
   void init_garbage_collector();
};

#endif