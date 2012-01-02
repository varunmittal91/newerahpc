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
#include <errno.h>

#include <include/thread.h>

using namespace std;

namespace neweraHPC
{
   thread_manager_t::thread_manager_t()
   {          
      active_threads = new rbtree_t;
      mutex = new pthread_mutex_t;
      pthread_mutex_init(mutex,NULL);
   }
   
   thread_manager_t::~thread_manager_t()
   {
      delete active_threads;
      delete mutex;
      pthread_mutex_destroy(mutex);
   }
   
   inline void thread_manager_t::lock()
   {
      if(mutex)
	 pthread_mutex_lock(mutex);
   }
   
   inline void thread_manager_t::unlock()
   {
      if(mutex)
	 pthread_mutex_unlock(mutex);
   }
   
   nhpc_status_t thread_manager_t::init_thread(int *thread_id, pthread_t **thread)
   {
      pthread_t *thread_new = new pthread_t;
      memset(thread_new, 0, sizeof(pthread_t));
      lock();
      (*thread_id) = (*active_threads).insert((void *)thread_new);
      unlock();
      
      if(thread)
	 (*thread) = thread_new;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t thread_manager_t::create_thread(int *thread_id, const pthread_attr_t *attr, 
						 void *(*start_routine)(void*), void *arg, int thread_state)
   {
      int rv;
      pthread_t *thread = (pthread_t *)(*active_threads).search(*thread_id);
      if(!thread)
	 return NHPC_FAIL;

      rv = pthread_create(thread, attr, start_routine, arg);
      if(rv != 0)
      {
	 perror("Thread Creation Failed");
	 return errno;
      }
      
      if(thread_state == NHPC_THREAD_JOIN)
      {
	 rv = pthread_join(*thread, NULL); 
	 if(rv != 0)
	 {
	    perror("Thread Creation Failed");
	    return errno;
	 }
      }
      else if(thread_state == NHPC_THREAD_DETACH)
      {
	 rv = pthread_detach(*thread);
	 if(rv != 0)
	 {
	    perror("Thread Creation Failed");
	    return errno;
	 }
      }      
      
      return NHPC_SUCCESS;
   }
   
   void thread_manager_t::delete_thread_data(int rbtree_t_id)
   {
      lock();
      pthread_t *thread = (pthread_t *)(*active_threads).search(rbtree_t_id);
      if(thread){
	 (*active_threads).erase(rbtree_t_id);
	 delete thread;
      }
      unlock();
   }
   
   int thread_manager_t::cancel_thread(int rbtree_t_id)
   {
      lock();
      pthread_t *thread = (pthread_t *)(*active_threads).search(rbtree_t_id);
      unlock();
      if(thread){
	 int status = pthread_cancel(*thread);
	 return status;
      }
      else 
	 return -1;
   }      
};
