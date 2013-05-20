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
#include <include/grid_plugin.h>

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
	 grid_data_add_result_data(grid_data, client_uid, size, ARG_MEM_BLOCK);
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
      if(nrv == NHPC_SUCCESS) 
      {
	 nhpc_size_t size = strlen(server_uid) + 1;
	 grid_data_add_result_data(grid_data, server_uid, size, ARG_MEM_BLOCK);
      }
      
      return nrv;
   }
   
   nhpc_status_t grid_plugin_request_handler(grid_data_t *grid_data)
   {
      nhpc_status_t     nrv;
      nhpc_size_t       size;
      plugin_details_t *plugin;
      
      rbtree     *headers     = grid_data->socket->headers;      
      const char *plugin_name = (const char *)headers->search("Plugin-Name");
      if(!plugin_name)
	 return NHPC_FAIL;
      
      nrv = grid_plugin_search(plugin_name, &plugin);
      if(nrv == NHPC_SUCCESS)
      {
	 const char *plugin_path;
	 
	 if(plugin->path_nxi)
	    plugin_path = plugin->path_nxi;
	 else 
	    plugin_path = plugin->path_plugin;
	 
	 grid_data_add_result_data(grid_data, plugin_path, size, ARG_FILE);
      }
      
      return nrv;
   }
   
   nhpc_status_t grid_plugin_exchange_handler(grid_data_t *grid_data)
   {
      nhpc_status_t     nrv;
      const char       *file_path;
      plugin_details_t *plugin;
      
      if(!(grid_data->input_data) || !(grid_arg_is_file(grid_data->input_data->arg)))
	 return NHPC_FAIL;
      
      file_path = (const char *)grid_shared_data_get_data_address(grid_data->input_data);
      nrv = grid_plugin_install_dll(file_path, &plugin);
      
      return nrv;
   }
   
   nhpc_status_t grid_file_exchange_request_handler(grid_data_t *grid_data)
   {

   }
}