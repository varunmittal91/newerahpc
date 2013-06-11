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

#include <include/grid_plugin.h>
#include <include/grid_default_plugins.h>
#include <include/grid_response.h>

namespace neweraHPC
{
   rbtree *plugins_installed;
   rbtree *plugins_requested;
   
   nhpc_mutex_t mutex_plugins_requested;
   nhpc_mutex_t mutex_plugins_installed;   
   
   void grid_plugin_system_init()
   {
      plugins_installed = new rbtree(RBTREE_STR);
      plugins_requested = new rbtree(RBTREE_STR);
      
      thread_mutex_init(&mutex_plugins_installed);
      thread_mutex_init(&mutex_plugins_requested);
      
      plugin_details_t *grid_plugin_range;
      grid_plugin_range_init(&grid_plugin_range);
      grid_plugin_install(grid_plugin_range);
   }
   
   nhpc_status_t grid_plugin_install(plugin_details_t *plugin_details)
   {
      nhpc_status_t nrv = NHPC_SUCCESS;
      
      if(!plugin_details || !(plugin_details->plugin_name))
	 return NHPC_FAIL;
      
      grid_plugin_lock_installed(NHPC_THREAD_LOCK_WRITE);
      if(grid_plugin_insert_installed(plugin_details, (plugin_details->plugin_name)) == 0)
      {
	 nrv = NHPC_FAIL;
      }
      grid_plugin_unlock_installed(NHPC_THREAD_LOCK_WRITE);
      
      return nrv;
   }
   
   nhpc_status_t grid_plugin_install_dll(const char *dll_path, plugin_details_t **plugin_details)
   {
      void *dll;
      fnc_ptr_nhpc_t    plugin_init_fnc;
      nhpc_status_t     nrv;
      
      if((dll = dlopen(dll_path, RTLD_NOW)) == NULL)
	 return errno;
      if((plugin_init_fnc = (fnc_ptr_nhpc_t)dlsym(dll, "plugin_init")) == NULL)
	 return errno;
      
      if((nrv = plugin_init_fnc(plugin_details)) != NHPC_SUCCESS)
	 return nrv;

      (*(*plugin_details)).path_plugin = dll_path;
      nrv = grid_plugin_install(*plugin_details);
      return nrv;
   }
   
   nhpc_status_t grid_plugin_search(const char *plugin_name, plugin_details_t **plugin_details)
   {
      (*plugin_details) = grid_plugin_search_installed(plugin_name);
      if(*plugin_details)
	 return NHPC_SUCCESS;
      
      plugin_request_t *plugin_request = grid_plugin_search_requested(plugin_name);
      if(plugin_request)
      {
	 if(grid_plugin_request_is_failed(plugin_request))
	    return NHPC_FAIL;
	 
	 thread_mutex_lock(&(plugin_request->mutex), NHPC_THREAD_LOCK_READ);
	 while(!grid_plugin_request_is_complete(plugin_request))
	 {
	    sleep(1);
	 }
	 thread_mutex_unlock(&(plugin_request->mutex), NHPC_THREAD_LOCK_READ);
      }
      
      (*plugin_details) = grid_plugin_search_installed(plugin_name);
      if(*plugin_details)
	 return NHPC_SUCCESS;      
      
      return NHPC_FAIL;
   }
   
   nhpc_status_t grid_plugin_request_plugin(const char *plugin_name, const char *peer_addr, const char *peer_port, plugin_details_t **plugin_details)
   {
      nhpc_status_t       nrv;
      const char         *plugin_path;
      grid_shared_data_t *data;
      
      plugin_request_t *plugin_request;
      grid_plugin_request_init(&plugin_request);
      grid_plugin_request_set_plugin_name(plugin_request, plugin_name);
      grid_plugin_request_set_peer(plugin_request, peer_addr, peer_port);
      
      grid_plugin_insert_requested(plugin_request);
      
      grid_communication_t *communication;
      grid_communication_init(&communication, GRID_PLUGIN_REQUEST);
      grid_communication_add_dest(communication, peer_addr, peer_port);
      grid_communication_set_header(communication, "Plugin-Name", plugin_name);
      
      grid_communication_send(communication);
      nrv = grid_communication_push(communication);
      if(nrv != NHPC_SUCCESS)
	 return nrv;
      
      grid_response_t *response;
      nrv = grid_response_get(&response, communication);
      grid_plugin_request_set_complete(plugin_request);
      if(nrv == NHPC_SUCCESS)
      {
	 if(grid_response_is_successful(response))
	 {
	    data        = response->data;
	    plugin_path = (const char *)grid_shared_data_get_data_address(data);
	    nrv = grid_plugin_install_dll(plugin_path, plugin_details);
	    if(nrv == NHPC_SUCCESS)
	    {
	       grid_plugin_request_set_successful(plugin_request);	    
	       grid_plugin_request_destruct(plugin_request);
	    }
	    else 
	       grid_plugin_request_set_failed(plugin_request);
	 }
	 else 
	 {
	    grid_plugin_request_set_failed(plugin_request);
	    nrv = NHPC_FAIL;
	 }
      }
      
      return nrv;
   }
   
   nhpc_status_t grid_plugin_exchange(const char *peer_addr, const char *peer_port, const char *plugin_path)
   {
      nhpc_status_t nrv;
      nhpc_size_t   size;
      
      grid_shared_data_t *data;
      grid_shared_data_init(&data);
      grid_shared_data_set_data(data, plugin_path, &size, ARG_FILE);
      
      grid_communication_t *communication;
      grid_communication_init(&communication, GRID_PLUGIN_EXCHANGE);
      grid_communication_add_dest(communication, peer_addr, peer_port);      
      grid_communication_add_data(communication, data);
      grid_communication_send(communication);
      nrv = grid_communication_push(communication);
      
      if(nrv == NHPC_SUCCESS)
      {
	 grid_response_t *response;
	 nrv = grid_response_get(&response, communication);
	 grid_response_destruct(response);
      }
      
      grid_communication_destruct(communication);
      
      return nrv;
   }
};