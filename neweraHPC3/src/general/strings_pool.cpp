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

#include <iostream>
#include <math.h>
#ifdef linux
#include <stdlib.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/strings.h>
#include <include/strings_pool.h>
#include <include/constants.h>
#include <include/error.h>
#include <include/alloc.h>

using namespace std;

namespace neweraHPC{
   /*
   bool garbage_collector_ready = false;
   
   void init_garbage_collector()
   {
#ifdef ENABLE_GARBAGE_COLLECTOR
      //garbage_collector_ready = true;
#endif
   }
   
   strings_pool_t::strings_pool_t()
   {
      strings_free = new rbtree(NHPC_RBTREE_NUM_HASH);
      strings_allocated = new rbtree(NHPC_RBTREE_NUM_HASH);
      
      mutex_free = new nhpc_mutex_t;
      thread_mutex_init(mutex_free);
      
      mutex_allocated = new nhpc_mutex_t;
      thread_mutex_init(mutex_allocated);
      
      free_count = 0;
      allocated_count = 0;
      
      free_bytes = 0;
      allocated_bytes = 0;
   }
   
   strings_pool_t::~strings_pool_t()
   {
      garbage_collector_ready = false;
      
      LOG_INFO("Shuting Down String Pool");
      
      delete strings_free;
      delete strings_allocated;
      
      LOG_INFO("String Pool closed");
   }
   
   char *strings_pool_t::search_string(nhpc_size_t str_len)
   {
      LOG_DEBUG("Searching for available string" << str_len);
      
      char *string = NULL;
      char *str = NULL;
      
      nhpc_size_t try_len_limit = str_len + 5;
      nhpc_size_t try_len = str_len;
      
      thread_mutex_lock(mutex_free, NHPC_THREAD_LOCK_WRITE);      	 
      do 
      {
	 string = (char *)strings_free->search(try_len, 1);
	 
	 if(string != NULL)
	 {
	    int ret = strings_free->erase(try_len, string);
	    LOG_DEBUG("FOUND EXISTING STRING AND REMOVED FROM FREE LIST with return status" << ret << " and size: " << try_len<< " original size: " << str_len);
	    if(ret == 0)
	       return NULL;
	    
	    char *string_zero = string + sizeof(nhpc_size_t);
	    
	    free_count--;
	    free_bytes -= try_len;
	    
	    break;
	 }
	 
	 try_len++; 
      }while(try_len < try_len_limit);
      thread_mutex_unlock(mutex_free, NHPC_THREAD_LOCK_WRITE);      
      
      bool allocate_new = false;
      
      if(string == NULL)
      {
	 LOG_DEBUG("ALLOCATING NEW STRING");
	 
	 string = (char *)malloc((sizeof(nhpc_size_t) + sizeof(char) * str_len));
	 
	 nhpc_size_t *str_len_string = (nhpc_size_t *)string;
	 *str_len_string = str_len;
	 try_len = str_len;
	 
	 allocate_new = true;
      }
      
      str = (char *)(string + sizeof(nhpc_size_t));
      
      thread_mutex_lock(mutex_allocated, NHPC_THREAD_LOCK_WRITE);
      strings_allocated->insert(string, try_len);      
      allocated_bytes += (sizeof(nhpc_size_t) + sizeof(char) * str_len);
      allocated_count++;
      if(!allocate_new)
      {
	 free_bytes -= (sizeof(nhpc_size_t) + sizeof(char) * str_len);
      }
      
      LOG_DEBUG("Free count: " << free_count << " Allocated count: " << allocated_count);
      
      thread_mutex_unlock(mutex_allocated, NHPC_THREAD_LOCK_WRITE);
      
      return str;
   }
   
   void strings_pool_t::free_string(char *str_address)
   {
      if(!str_address)
	 return;
      
      char *str = str_address - sizeof(nhpc_size_t);      
      nhpc_size_t *str_len_string = (nhpc_size_t *)(str);
      LOG_DEBUG("Length of string input: " << *str_len_string);
      
      if(*str_len_string < 0)
      {
	 LOG_ERROR("Invalid memory allocation");
	 return;
      }
      else if(*str_len_string == 0)
      {
	 LOG_ERROR("Not in strings pool");
	 return;
      }
      else 
      {
	 memset(str_address, 0, *str_len_string);
      }
      
      thread_mutex_lock(mutex_allocated, NHPC_THREAD_LOCK_WRITE);
      int ret = strings_allocated->erase(*str_len_string, str);
      if(ret != 0)
      {
	 allocated_count--;
	 allocated_bytes = allocated_bytes - (*str_len_string) - sizeof(nhpc_size_t);
	 free_bytes = free_bytes + (*str_len_string) + sizeof(nhpc_size_t);
	 
	 strings_free->insert(str, *str_len_string);	 
	 free_count++;
	 
	 LOG_DEBUG("ADDING STRING TO FREE POOL erased with return status " << ret << " Allocated bytes: " << allocated_bytes << " Free bytes: "<< free_bytes);
      }
      else 
      {
	 LOG_ERROR("String not erased");
      }
      LOG_DEBUG("Free count: " << free_count << " Allocated count: " << allocated_count);
      
      thread_mutex_unlock(mutex_allocated, NHPC_THREAD_LOCK_WRITE);
      
      //clean_strings();
   }
   
   void strings_pool_t::clean_strings()
   {
      thread_mutex_lock(mutex_free, NHPC_THREAD_LOCK_WRITE);
      
      //LOG_ERROR("Free count: " << free_count << " allocated count: " << allocated_count);
      
      if(free_count > MAX_STRING_COUNT)
      {	 
	 free_count = 0;
	 
	 int key;
	 bool exit_loop;
	 void *node_data;
	 
	 while(1)
	 {
	    node_data = strings_free->search_first(&key);
	    
	    if(!node_data)
	       break;
	    
	    char *string;
	    
	    while(1)
	    {
	       string = (char *)strings_free->search(key, 1);
	       if(string)
	       {
		  free(string);
		  
		  strings_free->erase(key, 1);
	       }
	       else 
	       {
		  exit_loop = true;
		  break;
	       }
	    }
	    
	    strings_free->erase(key);
	 }
      }
      
      thread_mutex_unlock(mutex_free, NHPC_THREAD_LOCK_WRITE);
   }
    */
}