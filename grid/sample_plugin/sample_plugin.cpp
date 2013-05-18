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

#include <neweraHPC/constants.h>

#include <include/grid_plugin.h>
#include <include/grid_instruction_set.h>

using namespace std;

namespace neweraHPC
{
#ifdef __cplusplus
   extern "C" 
   {
      nhpc_status_t plugin_exec(grid_instruction_t *instruction)
      {
      }
      
      nhpc_status_t plugin_processor(grid_instruction_t *instruction)
      {
	 const char *result = "Successful execution";
	 nhpc_size_t size   = strlen(result) + 1;
	 
	 grid_instruction_set_result_data(instruction, result, &size, ARG_MEM_BLOCK);
	 
	 return NHPC_SUCCESS;
      }      

      nhpc_status_t plugin_init(plugin_details_t **plugin_details)
      {
	 grid_plugin_details_init(plugin_details);
	 grid_plugin_details_set_plugin_name((*plugin_details), "GRID_SAMPLE_PLUGIN");
	 grid_plugin_details_set_fnc_exec((*plugin_details), plugin_exec);
	 grid_plugin_details_set_fnc_processor((*plugin_details), plugin_processor);
	 
	 return NHPC_SUCCESS;
      }
   }
#endif
};
