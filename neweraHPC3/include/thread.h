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

//! Create thread with default state
#define NHPC_THREAD_DEFAULT 0
//! Create thread and wait for termination
#define NHPC_THREAD_JOIN    1
//! Create thread and reclaim resources of thread after it dies
#define NHPC_THREAD_DETACH  2

namespace neweraHPC
{
   //! Thread Manager
   /*!
    Class for managing threads. All thread creation and management should be done through this class.
    */   
   class thread_manager_t
   {
   private:
      //! Active threads
      /*! RBTree data type keeping information of running threads 
       \sa rbtree_t
       */
      rbtree_t *active_threads;
      
      //! Mutex varibale
      pthread_mutex_t *mutex;
   public:  
      thread_manager_t();
      ~thread_manager_t();
      
      static void exit_handler(int sig);
      
      //! Mutex lock routine
      inline void lock();
      
      //! Mutex unlock routine
      inline void unlock();
      
      //! Thread data creation & initiation
      /*! Create & initialize thread data 
       \param thread_id as integer pointer must for thread_id
       \param thread as pthread_t pointer optional
       */
      nhpc_status_t init_thread(int *thread_id, pthread_t **thread);
      
      //! Thread creation & initiation
      /*! Create new thread on request. 'thread_state' to tell whether to join thread or dettach or do nothing. 
       \param thread_id an integer pointer must
       \param attr as pthread_attr_t optional
       \param start_routine must
       \param thread_state can be NHPC_THREAD_DEFAULT, NHPC_THREAD_JOIN, NHPC_THREAD_DETACH
       \sa NHPC_THREAD_DEFAULT
       \sa NHPC_THREAD_JOIN
       \sa NHPC_THREAD_DETACH
       */
      nhpc_status_t create_thread(int *thread_id, const pthread_attr_t *attr, 
				  void *(*start_routine)(void*), void *arg, int thread_state);
      //! Thread data deletion
      /*! Delete any data allocated for the thread */
      void delete_thread_data(int rbtree_t_id);

      //! Thread cancelation routine
      int cancel_thread(int rbtree_t_id);

      //! Thread kill
      int kill_thread(int rbtrr_id);
      
      //! Check for thread exit
      int exit_thread();
   };
};

#endif