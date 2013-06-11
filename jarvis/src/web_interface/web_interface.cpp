/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com>
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

#include <neweraHPC/neweraHPC.h>
#include <neweraHPC/constants.h>
#include <neweraHPC/grid_server.h>
#include <neweraHPC/grid_instruction.h>

#include <include/web_interface.h>

using namespace neweraHPC;

namespace jarvis
{
   void web_interface_init()
   {
      http_handler_register_app("jarvis", web_interface_handler);
   }
   
   void web_interface_handler(http_data_t *http_data)
   {
      cout << http_data->request << endl;
      
      string_t *words = nhpc_substr((http_data->request), '/');
      
      nhpc_status_t nrv;
      
      grid_instruction_t *instruction;
      grid_instruction_init(&instruction);
      grid_instruction_set_plugin_name(instruction, "JARVIS_GRID_PLUGIN");
      grid_instruction_set_peer(instruction, _host_addr, _host_port);
      grid_instruction_set_affinity(instruction, 2);
      for(int i = 1; i < 3; i++)
      {
	 char *word = words->strings[i];
	 grid_instruction_add_argument(instruction, ARG_LITERAL, word);
      }
      nrv = grid_instruction_send(instruction);  

      grid_shared_data_t *data;
      const char         *response;
      nhpc_size_t         response_len;
      
      if(nrv == NHPC_SUCCESS && (instruction->result_data) != NULL)
      {
	 data         = instruction->result_data;
	 response     = (const char *)data->address;
	 response     = nhpc_strconcat((const char *)(data->address));
	 response_len = strlen(response);
      }
      else 
      {
	 response     = nhpc_strconcat("{error}");
	 response_len = strlen(response);
      }
      http_data_add_content(http_data, (void *)response, "application/json", response_len, HTTP_CONTENT_TYPE_MEM, HTTP_OPT_NEW_ALLOCATED);
      
      delete[] response;
      
      grid_instruction_destruct(instruction);      
      
      nhpc_string_delete(words);
   }
};
