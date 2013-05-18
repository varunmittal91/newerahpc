/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
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

#include <include/grid_scheduler.h>
#include <include/grid_server.h>
#include <include/grid_node.h>
#include <include/grid_controller_register.h>
#include <include/grid_instruction_set.h>
#include <include/grid_client_instruction_generator.h>

namespace neweraHPC
{
   rbtree *queued_instructions;
   rbtree *jobs;
   nhpc_mutex_t mutex_queued_instructions;
   nhpc_mutex_t mutex_jobs;
   
   void grid_scheduler_system_init()
   {
      grid_controller_register_to_server(&_host_grid_uid, _host_addr, _host_port);
      
      queued_instructions = new rbtree(RBTREE_NUM_MANAGED);
      jobs                = new rbtree(RBTREE_STR);

      thread_mutex_init(&mutex_queued_instructions);
      thread_mutex_init(&mutex_jobs);
      
      int thread_id;
      (*_thread_manager).init_thread(&thread_id, NULL);
      (*_thread_manager).create_thread(&thread_id, NULL, (void* (*)(void*))grid_scheduler_job_dispatcher, NULL, NHPC_THREAD_DEFAULT);
   }
   
   nhpc_status_t grid_scheduler_queue_instruction(grid_instruction_t *instruction, grid_node_t *node)
   {
      const char *peer_addr = grid_node_get_peer_addr(node);
      const char *peer_port = grid_node_get_peer_port(node);      
      grid_instruction_set_peer(instruction, peer_addr, peer_port);
      
      int thread_id;
      (*_thread_manager).init_thread(&thread_id, NULL);
      (*_thread_manager).create_thread(&thread_id, NULL, (void* (*)(void *))grid_scheduler_dispatch_instruction, instruction, NHPC_THREAD_DEFAULT);
      
      return NHPC_SUCCESS;
   }
   
   void grid_scheduler_dispatch_instruction(grid_instruction_t *instruction)
   {
      nhpc_status_t  nrv;
      grid_node_t   *node;
 
      node = grid_node_search_compute_node(instruction->peer_uid);
      nrv = grid_instruction_send(instruction);
      if((nrv = grid_instruction_send(instruction)) != NHPC_SUCCESS)
      {
	 grid_node_free_compute_node(node, 1);
	 grid_instruction_unset_sent(instruction);
	 delete[] (instruction->peer_uid);
	 (instruction->peer_uid) = NULL;
      }
   }
   
   void *grid_scheduler_job_dispatcher()
   {
      int                 instruction_count;
      grid_instruction_t *instruction;
      grid_node_t        *node;
      nhpc_status_t       nrv;
      
      while(1)
      {
	 sleep(1);
	 
	 thread_mutex_lock(&mutex_queued_instructions, NHPC_THREAD_LOCK_WRITE);
	 instruction_count = (*queued_instructions).length();
	 
	 for(int i = 1; i <= instruction_count; i++)
	 {
	    instruction = (grid_instruction_t *)(*queued_instructions).search(i);
	    if(grid_instruction_is_sent(instruction))
	       continue;
	 
	    node = grid_node_get_compute_node(1);
	    if(node)
	    {
	       grid_instruction_set_peer_uid(instruction, node->peer_uid);	       
	       grid_instruction_set_sent(instruction);
	       grid_scheduler_queue_instruction(instruction, node);
	    }
	 }
	 thread_mutex_unlock(&mutex_queued_instructions, NHPC_THREAD_LOCK_WRITE);
      }
   }
   
   void grid_task_add_task(const char *grid_uid, grid_instruction_t **instructions, int *instructions_count)
   {
      grid_task_t *task = new grid_task_t;
      task->count        = *instructions_count;
      task->instructions = instructions;
      
      thread_mutex_lock(&mutex_jobs, NHPC_THREAD_LOCK_WRITE);
      jobs->insert(task, grid_uid);
      thread_mutex_unlock(&mutex_jobs, NHPC_THREAD_LOCK_WRITE);
      
      thread_mutex_lock(&mutex_queued_instructions, NHPC_THREAD_LOCK_WRITE);
      for(int i = 0; i < *instructions_count; i++)
	 queued_instructions->insert(instructions[i]);
      thread_mutex_unlock(&mutex_queued_instructions, NHPC_THREAD_LOCK_WRITE);
   }
   
   nhpc_status_t grid_scheduler_add_job(const char *grid_uid, grid_instruction_t **instructions, int *instructions_count)
   {
      grid_task_add_task(grid_uid, instructions, instructions_count);
   }
};