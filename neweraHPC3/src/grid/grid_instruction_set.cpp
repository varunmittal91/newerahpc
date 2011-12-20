/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
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

#include <include/grid.h>
#include <include/network.h>

namespace neweraHPC
{
   nhpc_status_t nhpc_create_instruction(nhpc_instruction_set_t **instruct_set, const char *plugin_name)
   {
      if(plugin_name == NULL)
	 return NHPC_FAIL;
      
      *instruct_set = new nhpc_instruction_set_t;
      
      (*instruct_set)->data = NULL;
      (*instruct_set)->data_len = 0;
      nhpc_strcpy(&((*instruct_set)->plugin_name), plugin_name);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_delete_instruction(nhpc_instruction_set_t *instruct_set)
   {
      delete[] instruct_set->plugin_name;
      delete instruct_set;
   }
};