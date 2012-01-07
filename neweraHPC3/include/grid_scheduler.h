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
   class grid_scheduler_t;
   
   class grid_scheduler_t
   {
   private:
      struct scheduler_thread_data_t
      {
	 grid_scheduler_t *scheduler;
	 nhpc_instruction_set_t *instruction_set;
	 thread_manager_t *thread_manager;
	 int thread_id;
      };
      
      rbtree_t *peers;
      rbtree_t *jobs;
      rbtree_t *queued_instructions;
      rbtree_t *child_processes;
      pthread_mutex_t *mutex;
      thread_manager_t **thread_manager;
      pthread_mutex_t *mutex_child_processes;
      
      void lock_child_processes();
      void unlock_child_processes();
      
      nhpc_status_t free_child_process();
      
      static void *job_dispatcher(scheduler_thread_data_t *data);
      
   public:
      grid_scheduler_t(thread_manager_t **_thread_manager);
      ~grid_scheduler_t();
      void grid_scheduler_init();
      int cores();
      void add_peer(const char *host, const char *port, int processors);
      void remove_peer(int peer_id);
      peer_details_t *schedule();
      void free_peer(int id);
      nhpc_status_t queue_job(nhpc_instruction_set_t *instruction_set);
      nhpc_status_t dispatch_job(nhpc_instruction_set_t *instruction_set);
      nhpc_status_t push_jobs();
      void lock();
      void unlock();
      
      nhpc_status_t add_child_process(nhpc_instruction_set_t *instruction_set, pid_t *pid);

      static void monitor_jobs_pending(grid_scheduler_t *grid_scheduler);
      static void child_handler(int signum);
   };   
};

#endif
