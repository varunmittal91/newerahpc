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
   }
   
   nhpc_status_t grid_plugin_install_dll(const char *dll_path, plugin_details_t **plugin_details)
   {
      
   }
   
   nhpc_status_t grid_plugin_search(const char *plugin_name, plugin_details_t **plugin_details)
   {
      (*plugin_details) = grid_plugin_search_installed(plugin_name);
      if(*plugin_details)
	 return NHPC_SUCCESS;
      
      plugin_request_t *plugin_request = grid_plugin_search_requested(plugin_name);
      if(plugin_request)
      {
	 while(grid_plugin_request_is_complete(plugin_request))
	 {
	    sleep(1);
	 }
      }
      
      (*plugin_details) = grid_plugin_search_installed(plugin_name);
      if(*plugin_details)
	 return NHPC_SUCCESS;      
      
      return NHPC_FAIL;
   }
};