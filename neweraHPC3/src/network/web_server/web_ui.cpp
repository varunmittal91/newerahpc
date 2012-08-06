/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
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
#include <fstream>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/network.h>
#include <include/general.h>
#include <include/web_ui.h>
#include <include/constants.h>

using namespace std;

namespace neweraHPC
{
   rbtree_t *app_handlers;
   
   nhpc_status_t web_ui_init()
   {
      app_handlers = new rbtree_t(NHPC_RBTREE_STR);
   }
   
   nhpc_status_t web_ui_register(const char *app_name, fnc_ptr_nhpc_two_t func_trigger)
   {
      fnc_ptr_nhpc_two_t *func_trigger_local = new fnc_ptr_nhpc_two_t;
      (*func_trigger_local) = func_trigger;
      
      nhpc_status_t rv = app_handlers->insert(func_trigger_local, app_name);
      
      return rv;
   }
   
   void web_ui_init_request(nhpc_socket_t *sock, string_t *request, char **file_path)
   {
      string_t *app_details = nhpc_substr(request->strings[1], '/');
      if(app_details->count < 2)
      {
	 return;
      }
      
      fnc_ptr_nhpc_two_t *func_trigger_local = (fnc_ptr_nhpc_two_t *)app_handlers->search(app_details->strings[1]);
      
      if(!func_trigger_local)
      {
	 return;
      }
      
      (*func_trigger_local)(sock, file_path);
   }
};