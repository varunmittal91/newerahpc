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

#include <neweraHPC/strings.h>
#include <iostream>

#include <include/grid_uid.h>
#include <include/grid_node.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t grid_uid_generate(const char **uid, grid_data_t *grid_data, node_type_t node_mode)
   {
      rbtree *node_db;
      if(grid_node_is_type_compute(node_mode))
	 node_db = registered_nodes;
      else 
	 node_db = registered_clients;
      
      const char *random_string = nhpc_random_string(8);
      while((*node_db).search(random_string))
      {
	 delete[] random_string;
	 random_string = nhpc_random_string(8);
      }

      *uid = random_string;
      return NHPC_SUCCESS;      
   }
}