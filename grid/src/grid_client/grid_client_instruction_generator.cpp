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

#include <neweraHPC/system.h>
#include <neweraHPC/error.h>

#include <include/grid_communication.h>
#include <include/grid_response.h>
#include <include/grid_client_instruction_generator.h>
#include <include/grid_instruction.h>
#include <include/grid_node.h>

using namespace std;

namespace neweraHPC
{   
   
   nhpc_status_t grid_instruction_send(grid_instruction_t *instruction)
   {
      const char *peer_addr   = grid_instruction_get_peer_addr(instruction);
      const char *peer_port   = grid_instruction_get_peer_port(instruction);
      const char *grid_uid    = grid_instruction_get_grid_uid(instruction);
      const char *plugin_name = grid_instruction_get_plugin_name(instruction);
      
      if(!peer_addr || !peer_port)
	 return NHPC_FAIL;
      
      nhpc_status_t       nrv;
      grid_shared_data_t *data = NULL;
      
      if(instruction->plugin_path)
      {
	 nrv = grid_plugin_exchange(peer_addr, peer_port, (instruction->plugin_path));
	 if(nrv != NHPC_SUCCESS)
	    return nrv;
      }
      
      grid_communication_t *grid_communication = NULL;
      grid_response_t      *response           = NULL;
      
      grid_communication_init(&grid_communication, GRID_INSTRUCTION);
      grid_communication_add_dest(grid_communication, peer_addr, peer_port);
      grid_communication_set_opt(grid_communication, GRID_COMMUNICATION_OPT_REGISTER | GRID_COMMUNICATION_OPT_SEND_PEER_DETAILS);
      if(grid_instruction_get_input_data(instruction))
	 data = grid_instruction_get_input_data(instruction);
      
      if(instruction->arguments)
      {
	 const char *argument;
	 const char *argument_headers;
	 const char *num_str;
	 const char *count_str;
	 int         count;
	 
	 count     = grid_instruction_get_argument_count(instruction);
	 count_str = nhpc_itostr(count);
	 	 
	 grid_communication_set_header(grid_communication, "Argument-Count", count_str);
	 for(int i = 1; i <= count; i++)
	 {
	    num_str          = nhpc_itostr(i);
	    argument_headers = nhpc_strconcat("Argument", num_str);	    
	    argument         = grid_instruction_get_argument(instruction, i);
	    
	    grid_communication_set_header(grid_communication, argument_headers, argument);
	    
	    delete[] argument_headers;
	    delete[] num_str;
	 }
	 delete[] count_str;
      }      
      grid_communication_set_header(grid_communication, "Plugin-Name", plugin_name);

      if(grid_instruction_is_executable(instruction))
	 grid_communication_set_header(grid_communication, "Execution-State", "Ready");

      char *affinity_str = nhpc_itostr(instruction->affinity);
      grid_communication_set_header(grid_communication, "Affinity", affinity_str);
      delete[] affinity_str;
      
      grid_communication_send(grid_communication);
      nrv = grid_communication_push(grid_communication, data);
      if(nrv != NHPC_SUCCESS)
      {
	 cout << "communication failed" << endl;
	 cout << nrv << endl;
	 goto return_status;
      }
      
      grid_instruction_set_grid_uid(instruction, grid_communication_get_grid_uid(grid_communication));
      
      nrv = grid_response_get(&response, grid_communication);
      if(nrv == NHPC_SUCCESS && grid_response_is_successful(response))
      {
	 if(response->data)
	 {
	    instruction->result_data = response->data;
	    response->data = NULL;
	 }
	    
	 grid_instruction_set_processed(instruction);
	 cout << "Instruction processing complete" << endl;
      }
      else 
      {
	 grid_instruction_unset_sent(instruction);
	 cout << "No response recieved" << endl;      
      }

      if(instruction->peer_uid)
      {
	 grid_node_t *node = grid_node_search_compute_node(instruction->peer_uid);
	 grid_node_free_compute_node(node, (instruction->affinity));
      }
      
   return_status:
      if(grid_communication)
	 grid_communication_destruct(grid_communication);
      if(response)
	 grid_response_destruct(response);
      
      return nrv;
   }   
   
   nhpc_status_t grid_instruction_wait_for_completetion(grid_instruction_t *instrunction)
   {
      
   }
   
   nhpc_status_t grid_instruction_check_for_result(grid_instruction_t *instrunction)
   {
      nhpc_status_t nrv;
      
      grid_response_t *response;
   }
};
