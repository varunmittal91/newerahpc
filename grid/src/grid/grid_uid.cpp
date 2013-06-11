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
      grid_node_t *grid_node;
      
      rbtree *node_db;
      rbtree *headers = grid_data->socket->headers;
      if(grid_node_is_type_compute(node_mode))
      {
	 node_db = registered_nodes;
	 
	 const char *cpu_cores_str = (const char *)headers->search("Core-Count");
	 const char *total_mem     = (const char *)headers->search("Total-Mem");
	 const char *free_mem      = (const char *)headers->search("Free-Mem");
	 const char *peer_addr     = (const char *)headers->search("Peer-Host");
	 const char *peer_port     = (const char *)headers->search("Peer-Port");
	 
	 if(!cpu_cores_str || !total_mem || !free_mem || !peer_addr || !peer_port)
	    return NHPC_FAIL;
	 
	 grid_node_init(&grid_node, NODE_TYPE_COMPUTE);
	 grid_node_set_peer_details(grid_node, peer_addr, peer_port);
	 
	 grid_node_compute_t *grid_node_compute;
	 grid_node_compute_init(&grid_node_compute);
	 grid_node_compute_set_logical_cores(grid_node_compute, cpu_cores_str);
	 grid_node_compute_set_total_mem(grid_node_compute, total_mem);
	 grid_node_compute_set_free_mem(grid_node_compute, free_mem);
	 
	 grid_node_set_compute_node_data(grid_node, grid_node_compute);
      }
      else 
      {
	 node_db = registered_clients;
	 
	 grid_node_init(&grid_node, NODE_TYPE_CLIENT);
	 grid_node_set_peer_details(grid_node, grid_data_get_peer_addr(grid_data), grid_data_get_peer_port(grid_data));
      }
      
      const char *random_string = nhpc_random_string(8);
      while((*node_db).search(random_string))
      {
	 delete[] random_string;
	 random_string = nhpc_random_string(8);
      }

      *uid = random_string;
      grid_node_set_uid(grid_node, *uid);      
      (*node_db).insert(grid_node, *uid);
      
      return NHPC_SUCCESS;      
   }
}