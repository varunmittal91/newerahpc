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

#ifndef _THREAD_H_
#define _THREAD_H_

#include <signal.h>

#include "rbtree.h"

#define NHPC_THREAD_DEFAULT 0
#define NHPC_THREAD_JOIN    1
#define NHPC_THREAD_DETACH  2

namespace neweraHPC
{
   class thread_manager_t
   {
   private:
      rbtree_t *active_threads;
      pthread_mutex_t *mutex;
      struct sigaction act;
   public:  
      thread_manager_t();
      ~thread_manager_t();
      static void exit_handler(int sig);
      /* Lock and unlock pthread_mutex on request. */
      inline void lock();
      inline void unlock();
      /* Create new thread on request. 'thread_state' to tell whether to join thread or dettach or do nothing. */
      nhpc_status_t init_thread(int *thread_id, pthread_t **thread);
      nhpc_status_t create_thread(int *thread_id, const pthread_attr_t *attr, 
				  void *(*start_routine)(void*), void *arg, int thread_state);
      void delete_thread_data(int rbtree_t_id);
      /* Needs working not yet functional */
      int cancel_thread(int rbtree_t_id);
      int kill_thread(int rbtrr_id);
      int exit_thread();
   };
};

#endif