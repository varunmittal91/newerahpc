/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
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

#ifndef _GRID_SCHEDULER_H_
#define _GRID_SCHEDULER_H_

#include "grid_data.h"
#include "rbtree.h"

namespace neweraHPC
{
   typedef struct queue_t{
      int peer_id; 
      int front;
      int rear;
      int task_total;
      int task_completed;
   }queue_t;
   
   struct scheduler_thread_data_t
   {
      peer_details_t *peer_details;
      const char *host_grid_uid;
      nhpc_instruction_set_t *instruction_set;
   };
   
   class grid_scheduler_t
   {
   private:
      rbtree_t *peers;
      rbtree_t *jobs;
      pthread_mutex_t *mutex;
      thread_manager_t **thread_manager;
      
   public:
      grid_scheduler_t(thread_manager_t **_thread_manager);
      ~grid_scheduler_t();
      void add_peer(const char *host, const char *port, int processors);
      peer_details_t *schedule();
      void free_peer(int id);
      nhpc_status_t queue_job(nhpc_instruction_set_t *instruction_set, const char *host_grid_uid);
      void lock();
      void unlock();
      
      static void *scheduler_thread(scheduler_thread_data_t *data);
   };
};

#endif
