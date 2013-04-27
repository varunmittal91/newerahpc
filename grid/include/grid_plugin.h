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

#ifndef _GRID_PLUGIN_H_
#define _GRID_PLUGIN_H_

#include <pthread.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include <neweraHPC/rbtree.h>

#include "grid_data.h"

namespace neweraHPC 
{
   void grid_plugin_system_init();
   
   extern rbtree *plugins_installed;
   extern rbtree *plugins_requested;
   
   typedef nhpc_status_t (*grid_plugin_fnc_ptr_t)(grid_data_t *);
   struct plugin_details_t
   {
      char       *plugin_name;
      const char *plugin_dir;
      const char *path_plugin;
      const char *path_nxi;
      
      grid_plugin_fnc_ptr_t fnc_init;
      grid_plugin_fnc_ptr_t fnc_exec;
      grid_plugin_fnc_ptr_t fnc_processor;
   };   
   static void grid_plugin_details_init(plugin_details_t **plugin_details)
   {
      (*plugin_details) = new plugin_details_t;
      memset((*plugin_details), 0, sizeof(plugin_details_t));
   }
   static void grid_plugin_details_destruct(plugin_details_t *plugin_details)
   {
      delete[] (plugin_details->plugin_name);
      delete plugin_details;
   }
#define grid_plugin_details_set_plugin_name(p, n)    (nhpc_strcpy(&(p->plugin_name), n))
#define grid_plugin_details_set_fnc_exec(p, f)       (p->fnc_exec = f)
#define grid_plugin_details_set_fnc_processor(p, f)  (p->fnc_processor = f)
   
   nhpc_status_t grid_plugin_install_dll(const char *dll_path, plugin_details_t **plugin_details);
   
   typedef unsigned char plugin_request_status;
   struct plugin_request_t
   {
      char  *plugin;
      char  *peer_host;
      char  *peer_port;
   
      plugin_request_status  status;
   };   
#define grid_plugin_request_set_sent(p)      ((p)->status |= 1)
#define grid_plugin_request_set_complete(p)  ((p)->status |= 2)
#define grid_plugin_request_is_sent(p)       ((p)->status & 1)
#define grid_plugin_request_is_complete(p)   (((p)->status >> 1) & 1)
};

#endif
