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

#include <include/grid_plugin_range.h>
#include <include/grid_scheduler.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t grid_plugin_range_init(plugin_details_t **plugin_details)
   {
      grid_plugin_details_init(plugin_details);
      grid_plugin_details_set_plugin_name((*plugin_details), "GRID_PLUGIN_RANGE");
      grid_plugin_details_set_fnc_exec((*plugin_details), grid_plugin_range_exec);
      grid_plugin_details_set_fnc_processor((*plugin_details), grid_plugin_range_processor);
   }
   
   nhpc_status_t grid_plugin_range_exec(grid_instruction_t *instruction)
   {
      cout << "range exec accessed" << endl;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t grid_plugin_range_processor(grid_instruction_t *instruction)
   {
      
      cout << "range processor accessed" << endl;
      
      int  value1, value2;      
      int  instruction_count = 0;
      bool found_range       = false;
      grid_instruction_t **instructions;

      if(instruction->arguments)
      {
	 int arg_count = grid_instruction_get_argument_count(instruction);
	 arg_value_t arg_value;
	 arg_t       arg;
	 for(int i = 1; i <= arg_count; i++)
	 {
	    arg_value = grid_instruction_get_argument(instruction, i);
	    arg       = grid_arg_get_code(arg_value);
	 
	    if(grid_arg_is_range(arg))
	    {
	       found_range = true;
	       
	       grid_arg_get_range(arg_value, &value1, &value2);
	       cout << "Arg value found" << endl;
	       
	       cout << value1 << value2 << endl;
	       if(value2 > value1)
		  instruction_count = (value2 - value1) + 1;
	    }
	 }
      }
      
      if(!found_range || !instruction_count)
	 return NHPC_FAIL;
      
      const char *plugin_name = grid_instruction_get_plugin_name(instruction);
      const char *grid_uid    = grid_instruction_get_grid_uid(instruction);
      
      cout << "Creating instruction:" << instruction_count << endl;
      instructions = new grid_instruction_t* [instruction_count];
      for(int i = 0; i < instruction_count; i++)
      {
	 instructions[i] = new grid_instruction_t;
	 grid_instruction_init(&(instructions[i]));
	 grid_instruction_set_plugin_name(instructions[i], plugin_name);
	 
	 int value = value1 + i;
	 grid_instruction_add_argument(instructions[i], ARG_NUMBER, &value);
      }
      grid_scheduler_add_job(grid_uid, instructions, &instruction_count);
      
      return NHPC_SUCCESS;
   }
};