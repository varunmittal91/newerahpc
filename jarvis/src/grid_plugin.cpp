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
#include <neweraHPC/json.h>
#include <neweraHPC/grid_scheduler.h>

#include <include/grid_plugin.h>
#include <include/parse_index.h>
#include <include/jarvis.h>
#include <include/jarvis_data.h>
#include <include/compare.h>

using namespace std;

namespace jarvis
{
#ifdef __cplusplus
   extern "C" 
   {
      nhpc_status_t plugin_init(plugin_details_t **plugin_details)
      {
	 init_jarvis_data();
	 thread_manager = new thread_manager_t;
	 
	 grid_plugin_details_init(plugin_details);
	 grid_plugin_details_set_plugin_name((*plugin_details), "JARVIS_GRID_PLUGIN");
	 grid_plugin_details_set_fnc_exec((*plugin_details), plugin_exec);
	 grid_plugin_details_set_fnc_processor((*plugin_details), plugin_processor);
	 
	 return NHPC_SUCCESS;
      }
      
      nhpc_status_t plugin_exec(grid_instruction_t *instruction)
      {
	 json_t *word_structure1 = NULL, *word_structure2 = NULL;
	 const char *word1 = NULL, *word2 = NULL;
	 
	 nhpc_status_t  nrv;
	 rbtree        *arguments = instruction->arguments;
	 if(arguments)
	 {	    
	    arg_t        arg;
	    arg_value_t  arg_value;
	    const char  *word;
	    
	    int argument_count = (*arguments).length();
	    for(int i = 1; i <= argument_count; i++)
	    {
	       arg_value = grid_instruction_get_argument(instruction, i);
	       arg       = grid_arg_get_code(arg_value);
	       grid_arg_get_literals(&word, arg_value);
	       
	       nrv = jv_get_word_def(word);
	       if(nrv == NHPC_SUCCESS)
	       {
		  jv_extract_word_def(word);
		  if(i == 1)
		  {
		     word1           = word;
		     word_structure1 = jv_get_json_structure(word);
		  }
		  else if(i == 2)
		  {
		     word2           = word;
		     word_structure2 = jv_get_json_structure(word);
		  }
	       }	       
	    }
	 }
	 
	 json_t *result = NULL;
	 if(word_structure1 && word_structure2)
	 {
	    cout << "Words loaded now comparing" << endl;
	    result = jv_compare_json_structure(word_structure1, word_structure2, word1, word2);
	    
	    const char  *str  = (*result).get_string();
	    nhpc_size_t  size = strlen(str) + 1;
	    grid_instruction_set_result_data(instruction, str, &size, ARG_MEM_BLOCK);
	 }

	 return NHPC_SUCCESS;
      }
      
      nhpc_status_t plugin_processor(grid_instruction_t *instruction)
      {
	 const char *plugin_name = grid_instruction_get_plugin_name(instruction);
	 const char *grid_uid    = grid_instruction_get_grid_uid(instruction);
	 const char *peer_addr   = grid_instruction_get_peer_addr(instruction);
	 const char *peer_port   = grid_instruction_get_peer_port(instruction);	 	 
      
	 int instruction_count = 1;
	 grid_instruction_t **instructions = new grid_instruction_t* [1];
	 grid_instruction_init(&(instructions[0]));
	 grid_instruction_set_plugin_name(instructions[0], plugin_name);
	 grid_instruction_set_executable(instructions[0]);	 

	 
	 grid_instruction_t *_instruction = instructions[0];
	 _instruction->arguments = instruction->arguments;
	 _instruction->affinity  = instruction->affinity;
	 
	 grid_scheduler_add_job(grid_uid, instructions, &instruction_count);
	 if(_instruction->result_data)
	 {
	    instruction->result_data  = _instruction->result_data;
	    _instruction->result_data = NULL;
	 }
	 cout << "job complete" << endl;
	 
	 return NHPC_SUCCESS;
      }      
   }
#endif
};

