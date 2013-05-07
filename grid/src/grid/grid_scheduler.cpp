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

namespace neweraHPC
{
   rbtree *queued_instructions;
   rbtree *jobs;
   
   void grid_scheduler_system_init()
   {
      queued_instructions = new rbtree(RBTREE_STR);
   }
   
   void grid_task_add_task(const char *grid_uid, grid_instruction_t **instructions, int *instructions_count)
   {
      grid_task_t *task = new grid_task_t;
      task->count        = *instructions_count;
      task->instructions = instructions;
      
      jobs->insert(task, grid_uid);
   }
   
   nhpc_status_t grid_scheduler_add_job(const char *grid_uid, grid_instruction_t **instructions, int *instructions_count)
   {
      grid_task_add_task(grid_uid, instructions, instructions_count);
   }
};