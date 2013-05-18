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

#ifndef _GRID_SCHEDULER_H_
#define _GRID_SCHEDULER_H_

#include <neweraHPC/constants.h>
#include <neweraHPC/rbtree.h>

#include "grid_data.h"
#include "grid_instruction_data_type.h"
#include "grid_node.h"

namespace neweraHPC
{
   struct grid_job_t
   {
      int     instructions_count;
      rbtree *instructions_queued;
   };
   
   extern rbtree *queued_instructions;
   extern rbtree *jobs;
   
   void grid_scheduler_system_init();
   
   struct grid_task_t
   {
      int count;
      grid_instruction_t **instructions;
   };
   static void grid_task_add_task(const char *grid_uid, grid_instruction_t **instructions, int *instructions_count);
   
   nhpc_status_t grid_scheduler_find_peer();
   nhpc_status_t grid_scheduler_add_job(const char *grid_uid, grid_instruction_t **instructions, int *instructions_count);
   void *grid_scheduler_job_dispatcher();
   nhpc_status_t grid_scheduler_queue_instruction(grid_instruction_t *instruction, grid_node_t *node);
   void grid_scheduler_dispatch_instruction(grid_instruction_t *instruction);
};

#endif
