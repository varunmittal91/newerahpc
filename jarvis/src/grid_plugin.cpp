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
#include <include/parse_json_structure.h>

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
	 json_t     *word_structure1 = NULL;
	 const char *word1           = NULL;
	 
	 nhpc_status_t  nrv;
	 rbtree        *arguments = instruction->arguments;
	 if(!arguments)
	    return NHPC_SUCCESS;
	 
	 arg_t        arg;
	 arg_value_t  arg_value;
	 const char  *word;
	    
	 arg_value = grid_instruction_get_argument(instruction, 1);
	 arg       = grid_arg_get_code(arg_value);
	 grid_arg_get_literals(&word, arg_value);

	 nrv = jv_get_word_def(word);
	 if(nrv == NHPC_SUCCESS)
	 {
	    word_structure1 = jv_get_json_structure(word);

	    const char  *json_str = (*word_structure1).get_string();
	    nhpc_size_t  size     = strlen(json_str) + 1;
	    grid_instruction_set_result_data(instruction, json_str, &size, ARG_MEM_BLOCK);
	 }
	 
	 return NHPC_SUCCESS;
      }
      
      nhpc_status_t plugin_processor(grid_instruction_t *instruction)
      {
	 const char *plugin_name = grid_instruction_get_plugin_name(instruction);
	 const char *grid_uid    = grid_instruction_get_grid_uid(instruction);
	 const char *peer_addr   = grid_instruction_get_peer_addr(instruction);
	 const char *peer_port   = grid_instruction_get_peer_port(instruction);	 	 
      
	 int instruction_count = 2;
	 grid_instruction_t **instructions = new grid_instruction_t* [2];
	 
	 for(int i = 0; i < instruction_count; i++)
	 {
	    grid_instruction_init(&(instructions[i]));
	    grid_instruction_set_plugin_name(instructions[i], plugin_name);
	    grid_instruction_set_executable(instructions[i]);	 
 
	    grid_instruction_t *_instruction = instructions[i];
	    _instruction->affinity = instruction->affinity;
	 }
	 
	 json_t *word_structure1 = NULL;
	 json_t *word_structure2 = NULL;

	 const char *words[2];
	 
	 rbtree *arguments = instruction->arguments;
	 if(instruction->arguments)
	 {
	    arg_t        arg;
	    arg_value_t  arg_value;
	    
	    int argument_count = (*arguments).length();
	    if(argument_count > 2)
	       argument_count = 2;
	    for(int i = 1; i <= argument_count; i++)
	    {
	       grid_arg_get_literals(&(words[i - 1]), arg_value);
	       
	       arg_value = grid_instruction_get_argument(instruction, i);
	       grid_instruction_add_argument(instructions[i - 1], arg_value);
	    }	       
	 }
	 
	 grid_scheduler_add_job(grid_uid, instructions, &instruction_count);
	 
	 if(!(instructions[0]->result_data) || !(instructions[0]->result_data))
	    return NHPC_SUCCESS;
	 
	 const char *word_str1 = (const char *)((instructions[0])->result_data->address);
	 const char *word_str2 = (const char *)((instructions[1])->result_data->address);

	 word_structure1 = new json_t;
	 word_structure2 = new json_t;
	 (*word_structure1).build_structure(word_str1);
	 (*word_structure2).build_structure(word_str2);
	 
	 json_t *result = match_json_structures(word_structure1, word_structure2);
	 if(result)
	 {
	    const char *result_str = (*result).get_string();
	    nhpc_size_t size       = strlen(result_str) + 1;
	    nhpc_strcpy((char **)&result_str, result_str);
	    
	    grid_instruction_set_result_data(instruction, result_str, &size, ARG_MEM_BLOCK);
	    
	    delete result;
	 }
	 delete   word_structure1;
	 delete   word_structure2;
	 delete[] word_str1;
	 delete[] word_str2;
	 
	 return NHPC_SUCCESS;
      }      
   }
#endif
};

