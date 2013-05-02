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

#include <include/grid_instruction_set.h>
#include <include/grid_communication.h>
#include <include/grid_response.h>

using namespace std;

namespace neweraHPC
{   
   void grid_instruction_destruct(grid_instruction_t *instruction)
   {
      if(grid_instruction_get_plugin_name(instruction))
	 delete[] grid_instruction_get_plugin_name(instruction);
      
      if(grid_instruction_get_grid_uid(instruction))
      	 delete[] grid_instruction_get_grid_uid(instruction);
            
      if(grid_instruction_get_referer_grid_uid(instruction))
	 delete[] grid_instruction_get_referer_grid_uid(instruction);
      
      if(grid_instruction_get_peer_addr(instruction))
	 delete[] grid_instruction_get_peer_addr(instruction);
      
      if(grid_instruction_get_peer_port(instruction))
	 delete[] grid_instruction_get_peer_port(instruction);
      
      if(grid_instruction_get_input_data(instruction))
      	 grid_shared_data_destruct(instruction->input_data);
      
      if(grid_instruction_get_result_data(instruction))
      	 grid_shared_data_destruct(instruction->result_data);
            
      if(instruction->arguments)
      {
	 int         argument_count = instruction->arguments->length();
	 const char *argument_value;

	 for(int i = 1; i <= argument_count; i++)
	 {
	    argument_value = (const char *)instruction->arguments->search(i);
	    delete[] argument_value;
	 }
	 
	 delete (instruction->arguments);
      }

      delete instruction;
   }
   
   nhpc_status_t grid_instruction_prepare(grid_instruction_t **instruction, grid_data_t *grid_data)
   {
      grid_instruction_init(instruction);
      
      nhpc_socket_t *socket  = grid_data->socket;
      rbtree        *headers = socket->headers;
      rbtree        *arguments;

      char *argument_count_str = (char *)headers->search("Argument-Count");
      if(argument_count_str)
      {
	 arguments = new rbtree(RBTREE_NUM);
	 
	 int   argument_count = nhpc_strtoi(argument_count_str); 
	 char *argument_num_str;
	 char *argument_str;
	 char *argument;
	 
	 for(int i = 1; i <= argument_count; i++)
	 {
	    argument_num_str = nhpc_itostr(i);
	    argument_str     = nhpc_strconcat("Argument", argument_num_str);
	    argument         = (char *)headers->search(argument_str);
	    
	    delete[] argument_num_str;
	    delete[] argument_str;
	    
	    arguments->insert(argument);
	 }
      }	 
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t grid_instruction_add_argument(grid_instruction_t *instruction, const char *arg_value)
   {
      if(!(instruction->arguments))
	 instruction->arguments = new rbtree(RBTREE_NUM);
      
      instruction->arguments->insert((void *)arg_value);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t grid_instruction_add_argument(grid_instruction_t *instruction, arg_t arg, 
					       const void *value1, const void *value2)
   {
      const char *arg_value = NULL;
      
      if(!(instruction->arguments))
	 instruction->arguments = new rbtree(RBTREE_NUM);
      
      switch(arg)
      {
	 case ARG_RANGE:
	    if(!value2)
	       return NHPC_FAIL;

	    grid_arg_set_range(&arg_value, *((int *)value1), *((int *)value2));
	    break;
	    
	 case ARG_NUMBER:
	    grid_arg_set_number(&arg_value,*((int *)value1));
	    break;
	    
	 case ARG_LITERAL:
	 case ARG_COMMAND:
	    grid_arg_set_literals(&arg_value, (const char *)value1, arg);
	    break;
	    
	 default:
	    return NHPC_FAIL;
      }
      
      instruction->arguments->insert((void *)arg_value);
      return NHPC_SUCCESS;
   }
};