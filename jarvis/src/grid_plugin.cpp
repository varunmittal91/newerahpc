/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *	jarvis program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of jarvis.
 *
 *	jarvis is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	jarvis is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with jarvis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <neweraHPC/error.h>

#include <include/grid_plugin.h>

using namespace std;

namespace jarvis
{
#ifdef __cplusplus
   extern "C" 
   {
      /*
      nhpc_status_t jarvis_grid_plugin_register()
      {
      }
   
      nhpc_status_t plugin_init(plugin_details_t **plugin_details)
      {
	 (*plugin_details)->fnc_exec        = (fnc_ptr_nhpc_plugin_t)plugin_exec;
	 (*plugin_details)->fnc_client_exec = (fnc_ptr_nhpc_plugin_t)plugin_client_exec;
	 (*plugin_details)->fnc_processor   = (fnc_ptr_nhpc_plugin_t)plugin_processor;
	 (*plugin_details)->plugin_name     = (char *)"JARVIS_GRID";
      
	 return NHPC_SUCCESS;
      }
   
      nhpc_status_t plugin_exec(nhpc_grid_server_t *grid_server, nhpc_instruction_set_t *instruction_set, 
				nhpc_socket_t *sock, char **grid_uid)
      {
	 cout << "JARVIS PLUGIN EXEC" << endl;

	 rbtree *headers = (rbtree *)sock->headers;
	 
	 int *argument_count = &(instruction_set->argument_count);
	 cout << *argument_count << endl;
	 for(int i = 1; i <= *argument_count; i++)
	 {
	    nhpc_instruction_set_t *new_instruction_set;
	    nhpc_generate_general_instruction(&new_instruction_set, headers);
	    
	    char *key      = nhpc_strconcat("Argument", nhpc_itostr(i));
	    char *argument = (char *)headers->search(key);
	    cout << "Argument:" << argument << endl;
	    
	    string_t *arg = nhpc_substr(argument, ',');
	    
	    char *word = nhpc_strconcat(arg->strings[1], ",", arg->strings[2]);
	    nhpc_add_argument(new_instruction_set, LITERAL, word);
	    new_instruction_set->execute = true;
	    grid_server->queue_job(new_instruction_set);
	    LOG_INFO("queueing job no: " << i);
	 }
	 
	 
	 return NHPC_SUCCESS;
      }
   
      nhpc_status_t plugin_client_exec(nhpc_grid_server_t *grid_server, nhpc_instruction_set_t *instruction_set, 
				       nhpc_socket_t *sock, char **grid_uid)
      {
	 cout << "JARVIS PLUGIN CLIENT EXEC" << endl;
	 
	 return NHPC_SUCCESS;
      }
   
      nhpc_status_t plugin_processor(nhpc_grid_server_t *grid_server, nhpc_instruction_set_t *instruction_set, 
				     nhpc_socket_t *sock, char **grid_uid)
      {
	 cout << "JARVIS PLUGIN PROCESSOR" << endl;
	 
	 return NHPC_SUCCESS;
      }
       */
   }
#endif
};

