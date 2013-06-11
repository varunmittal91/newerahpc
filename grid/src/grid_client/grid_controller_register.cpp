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

using namespace std;

namespace neweraHPC
{
   nhpc_status_t grid_controller_register_to_server(const char **grid_uid, const char *host_addr, const char *host_port)
   {
      nhpc_status_t nrv;
      
      const char *core_count_str = nhpc_itostr(nhpc_system_get_logical_cores());
      const char *total_mem_str  = nhpc_longitostr(nhpc_system_get_total_mem());
      const char *free_mem_str   = nhpc_longitostr(nhpc_system_get_free_mem());
      
      grid_communication_t *grid_communication;
      grid_communication_init(&grid_communication, GRID_NODE_REGISTRATION);
      grid_communication_set_opt(grid_communication, GRID_COMMUNICATION_OPT_SEND_PEER_DETAILS);
      grid_communication_add_dest(grid_communication, host_addr, host_port);

      grid_set_communication_header(grid_communication, "Core-Count", core_count_str);
      grid_set_communication_header(grid_communication, "Free-Mem", free_mem_str);     
      grid_set_communication_header(grid_communication, "Total-Mem", total_mem_str);
     
      grid_communication_send(grid_communication);
      nrv = grid_communication_push(grid_communication);   
      
      delete[] core_count_str;
      delete[] total_mem_str;
      delete[] free_mem_str;
      
      if(nrv != NHPC_SUCCESS)
      {
	 return nrv;
      }
      
      grid_response_t *grid_response;
      nrv = grid_response_get(&grid_response, grid_communication);
      if(nrv == NHPC_SUCCESS)
      {
	 if(grid_get_response_status_code(grid_response) != GRID_RESPONSE_SUCCESSFUL)
	    LOG_ERROR("Registration Failed");
	 else 
	 {
	    grid_shared_data_t *data = grid_response_get_grid_data(grid_response);
	    if(data && grid_response_is_successful(grid_response))
	    {
	       (*grid_uid) = (const char *)grid_shared_data_get_data_address(data);
	       cout << "Controller Registration code:" << (*grid_uid) << endl;	    
	    }
	    else 
	       nrv = NHPC_FAIL;
	 }
      }
      grid_communication_destruct(grid_communication);
      grid_response_destruct(grid_response);
      
      return nrv;      
   }
}