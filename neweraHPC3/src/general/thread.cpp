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

#include <include/thread.h>
#include <iostream>

using namespace std;

namespace neweraHPC
{
   thread_manager_t::thread_manager_t()
   {          
      active_threads = new rbtree;
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
   
   int thread_manager_t::create_thread(const pthread_attr_t *attr, 
		     void *(*start_routine)(void*), void *arg, int thread_state)
   {
      pthread_t *thread_new = new pthread_t;
      int status_new = pthread_create(thread_new, attr, start_routine, arg);
      if(status_new!=0){
	 perror("Thread creation failed");
	 return 0;
      }
      
      lock();
      int rbtree_id = (*active_threads).insert((void *)thread_new);
      unlock();
     
      if(thread_state == NHPC_THREAD_JOIN)
      {
	 status_new = pthread_join(*thread_new,NULL); 
	 if(status_new!=0)perror("Error at joing thread");
	 cout<<"joining thread for you";
      }
      else if(thread_state == NHPC_THREAD_DETACH)
      {
	 status_new = pthread_detach(*thread_new);
	 if(status_new!=0)perror("Error at detaching thread");
      }
      
      return rbtree_id;
   }
   
   void thread_manager_t::delete_thread_data(int rbtree_id)
   {
      lock();
      pthread_t *thread = (pthread_t *)(*active_threads).search(rbtree_id);
      if(thread){
	 (*active_threads).erase(rbtree_id);
	 delete thread;
      }
      unlock();
   }
   
   int thread_manager_t::cancel_thread(int rbtree_id)
   {
      lock();
      pthread_t *thread = (pthread_t *)(*active_threads).search(rbtree_id);
      unlock();
      if(thread){
	 int status = pthread_cancel(*thread);
	 return status;
      }
      else 
	 return -1;
   }   
};
