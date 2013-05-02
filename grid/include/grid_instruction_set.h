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

#ifndef _GRID_INSTRUCTION_SET_H_
#define _GRID_INSTRUCTION_SET_H_

#include "grid_communication.h"
#include "grid_data.h"

namespace neweraHPC
{
   typedef unsigned char instruction_status_t;
#define GRID_INSTRUCTION_SENT      1
#define GRID_INSTRUCTION_PROCESSED 2
   struct grid_instruction_t
   {
      const char *plugin_name;
      rbtree     *arguments;
      
      const char *grid_uid;
      const char *peer_addr;
      const char *peer_port;

      const char *referer_grid_uid;
      const char *referer_addr;
      const char *referer_port;
      
      grid_shared_data_t *input_data;
      grid_shared_data_t *result_data;
      
      instruction_status_t  instruction_status;
   };
   
   static bool grid_instrution_opt_is_set(grid_instruction_t *instruction, instruction_status_t opt)
   {
      int bit = 0;
      while(opt > 1)
      {
	 opt /= 2;
	 bit++;
      }
      
      return (1 & ((instruction->instruction_status) >> bit));
   }
#define grid_instruction_is_sent(i)       (grid_instruction_opt_is_set(i, GRID_INSTRUCTION_SENT))
#define grid_instruction_is_processed(i)  (grid_instruction_opt_is_set(i, GRID_INSTRUCTION_PROCESSED))
   
#define grid_instruction_get_input_data(i)       (i->input_data)
#define grid_instruction_get_result_data(i)      (i->result_data)
   
#define grid_instruction_get_plugin_name(i)      (i->plugin_name)
#define grid_instruction_get_peer_id(i)          (i->peer_id)
#define grid_instruction_set_peer_id(i, p)       (i->peer_id = p)
#define grid_instruction_get_peer_addr(i)        (i->peer_addr)
#define grid_instruction_get_peer_port(i)        (i->peer_port)
#define grid_instruction_get_grid_uid(i)         (i->grid_uid)
#define grid_instruction_get_referer_grid_uid(i) (i->referer_grid_uid)
#define grid_instruction_get_argument_count(i)   (i->arguments->length())
#define grid_instruction_get_argument(i, n)      ((const char *)i->arguments->search(n))
   static void grid_instruction_init(grid_instruction_t **instruction)
   {
      (*instruction) = new grid_instruction_t;
      memset((*instruction), 0, sizeof(grid_instruction_t));
      
      grid_instruction_t *tmp = (*instruction);
   }
   void grid_instruction_destruct(grid_instruction_t *instruction);
   static void grid_instruction_set_plugin_name(grid_instruction_t *instruction, const char *plugin_name)
   {
      nhpc_strcpy((char **)&(grid_instruction_get_plugin_name(instruction)), plugin_name);
   }
   static void grid_instruction_set_peer(grid_instruction_t *instruction, const char *peer_addr, const char *peer_port)
   {
      nhpc_strcpy((char **)&(grid_instruction_get_peer_addr(instruction)), peer_addr);
      nhpc_strcpy((char **)&(grid_instruction_get_peer_port(instruction)), peer_port);
   }
   static void grid_instruction_set_grid_uid(grid_instruction_t *instruction, const char *grid_uid)
   {
      nhpc_strcpy((char **)&(grid_instruction_get_grid_uid(instruction)), grid_uid);
   }
   static void grid_instruction_set_referer_grid_uid(grid_instruction_t *instruction, const char *referer_grid_uid)
   {
      nhpc_strcpy((char **)&(grid_instruction_get_referer_grid_uid(instruction)), referer_grid_uid);      
   }
   nhpc_status_t grid_instruction_prepare(grid_instruction_t **instruction, grid_data_t *grid_data);

   nhpc_status_t grid_instruction_add_argument(grid_instruction_t *instruction, const char *arg_value);
   nhpc_status_t grid_instruction_add_argument(grid_instruction_t *instruction, arg_t arg, 
					       const void *value1, const void *value2 = NULL);

   static void grid_instruction_set_data(grid_instruction_t *grid_instruction, void *address, nhpc_size_t *len,
					 arg_t arg, bool is_input)
   {
      grid_shared_data_t **data;
      
      if(is_input)
	 data = &(grid_instruction->input_data);
      else 
	 data = &(grid_instruction->result_data);
      
      grid_shared_data_init(data);
      grid_shared_data_set_data((*data), address, len, arg);
   }
#define grid_instruction_set_input_data(i, addr, l, a)    (grid_instruction_set_data(i, (void *)addr, l, a, true))
#define grid_instruction_set_result_data(i, addr, l, a)   (grid_instruction_set_data(i, (void *)addr, l, a, false))
#define grid_instruction_set_communication(i, c)          (i->communication = c)
   
}

#endif
