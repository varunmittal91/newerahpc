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

#include <include/grid_data.h>
#include <include/grid_node.h>
#include <include/grid_uid.h>
#include <include/grid_response.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t grid_client_registration_handler(grid_data_t *grid_data)
   {
      nhpc_status_t nrv;
      
      const char *client_uid;
      nrv = grid_uid_generate(&client_uid, grid_data, NODE_TYPE_CLIENT);
      if(nrv == NHPC_SUCCESS)
      {
	 nhpc_size_t size = strlen(client_uid) + 1;
	 grid_data_add_data(grid_data, client_uid, size, ARG_MEM_BLOCK);
	 return NHPC_SUCCESS;
      }
      else 
	 return NHPC_FAIL;
      
      return nrv;
   }
   
   nhpc_status_t grid_node_registration_handler(grid_data_t *grid_data)
   {
      nhpc_status_t nrv;
      
      const char *server_uid;
      nrv = grid_uid_generate(&server_uid, grid_data, NODE_TYPE_COMPUTE);
      if(nrv != NHPC_SUCCESS)
	 return NHPC_FAIL;
      else 
      {
	 nhpc_size_t size = strlen(server_uid) + 1;
	 grid_data_add_data(grid_data, server_uid, size, ARG_MEM_BLOCK);
	 return NHPC_SUCCESS;
      }
      
      return nrv;
   }
   
   nhpc_status_t grid_plugin_request_handler(grid_data_t *grid_data)
   {
      
   }
   
   nhpc_status_t grid_file_exchange_request_handler(grid_data_t *grid_data)
   {
      
   }
}