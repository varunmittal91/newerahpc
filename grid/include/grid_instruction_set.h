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

#include "grid_data.h"

namespace neweraHPC
{
   typedef unsigned char instruction_status_t;
   struct instruction_t
   {
      const char *plugin_name;
      const char *grid_uid;
      const char *referer_grid_uid;
      const char *peer_addr;
      const char *peer_port;
      int         peer_id;
      
      arg_value_t *input_data;
      arg_value_t *result_data;
      
      instruction_status_t instruction_status;
   };
#define instruction_get_input_data(i)       (i->input_data)
#define instruction_get_result_data(i)      (i->result_data)
#define instruction_get_plugin_name(i)      (i->plugin_name)
#define instruction_get_peer_id(i)          (i->peer_id)
#define instruction_set_peer_id(i, p)       (i->peer_id = p)
#define instruction_get_peer_addr(i)        (i->peer_addr)
#define instruction_get_peer_port(i)        (i->peer_port)
#define instruction_get_grid_uid(i)         (i->grid_uid)
#define instruction_get_referer_grid_uid(i) (i->referer_grid_uid)
   static void instruction_init(instruction_t **instruction)
   {
      (*instruction) = new instruction_t;
      memset((*instruction), 0, sizeof(instruction_t));
      
      instruction_t *tmp = (*instruction);
      grid_arg_init(&(instruction_get_input_data(tmp)));
      grid_arg_init(&(instruction_get_result_data(tmp)));
   }
   static void instruction_destruct(instruction_t *instruction)
   {
      if(instruction_get_plugin_name(instruction))
	 delete[] instruction_get_plugin_name(instruction);
      if(instruction_get_grid_uid(instruction))
	 delete[] instruction_get_grid_uid(instruction);
      if(instruction_get_referer_grid_uid(instruction))
	 delete[] instruction_get_referer_grid_uid(instruction);
      if(instruction_get_peer_addr(instruction))
	 delete[] instruction_get_peer_addr(instruction);
      if(instruction_get_peer_port(instruction))
	 delete[] instruction_get_peer_port(instruction);
      if(instruction_get_input_data(instruction))
      {
	 grid_arg_destruct(instruction_get_input_data(instruction));
	 delete instruction_get_input_data(instruction);
      }
      if(instruction_get_result_data(instruction))
      {
	 grid_arg_destruct(instruction_get_result_data(instruction));
	 delete instruction_get_result_data(instruction);
      }
   }
   static void instruction_set_plugin_name(instruction_t *instruction, const char *plugin_name)
   {
      nhpc_strcpy((char **)&(instruction_get_plugin_name(instruction)), plugin_name);
   }
   static void instruction_set_peer(instruction_t *instruction, int peer_id, const char *peer_addr, const char *peer_port)
   {
      instruction_set_peer_id(instruction, peer_id);
      nhpc_strcpy((char **)&(instruction_get_peer_addr(instruction)), peer_addr);
      nhpc_strcpy((char **)&(instruction_get_peer_port(instruction)), peer_port);
   }
   static void instruction_set_grid_uid(instruction_t *instruction, const char *grid_uid)
   {
      nhpc_strcpy((char **)&(instruction_get_grid_uid(instruction)), grid_uid);
   }
   static void instruction_set_referer_grid_uid(instruction_t *instruction, const char *referer_grid_uid)
   {
      nhpc_strcpy((char **)&(instruction_get_referer_grid_uid(instruction)), referer_grid_uid);      
   }
}

#endif
