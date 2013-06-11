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

#include <iostream>

#include <neweraHPC/network.h>
#include <neweraHPC/error.h>

#include <include/grid_data.h>
#include <include/grid_node.h>
#include <include/grid_uid.h>
#include <include/grid_response.h>
#include <include/grid_instruction.h>
#include <include/grid_plugin.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t grid_instruction_request_handler(grid_data_t *grid_data)
   {
      nhpc_status_t       nrv;
      grid_instruction_t *instruction;
      nrv = grid_instruction_prepare(&instruction, grid_data);
      if(nrv != NHPC_SUCCESS)
	 return nrv;

      const char *plugin_name = grid_instruction_get_plugin_name(instruction);
      const char *peer_addr   = grid_instruction_get_peer_addr(instruction);
      const char *peer_port   = grid_instruction_get_peer_port(instruction);
      const char *grid_uid    = grid_instruction_get_grid_uid(instruction);
      
      plugin_details_t *plugin_details;
      nrv = grid_plugin_search(plugin_name, &plugin_details);
      if(nrv == NHPC_FAIL && peer_addr && peer_port)
      {
	 nrv = grid_plugin_request_plugin(plugin_name, peer_addr, peer_port, &plugin_details);
      }
      if(nrv != NHPC_SUCCESS)
	 return nrv;
      
      if((nrv = grid_node_client_queue_job(grid_uid, instruction)) != NHPC_SUCCESS)
	 return nrv;
      
      nrv = grid_instruction_execute(instruction, plugin_details);
      if(instruction->result_data)
      {
	 grid_data->result_data = instruction->result_data;
	 instruction->result_data = NULL;
      }
      
      grid_node_delete_client_node(grid_uid);
      grid_instruction_destruct(instruction);
      
      return nrv;
   }   
};