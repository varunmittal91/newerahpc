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

#include <neweraHPC/rbtree.h>
#include <neweraHPC/thread.h>

#include <include/grid_node.h>

namespace neweraHPC
{
   rbtree *registered_nodes;
   rbtree *registered_clients;
   nhpc_mutex_t mutex_registered_nodes;
   nhpc_mutex_t mutex_registered_clients;
   
   grid_node_t *grid_node_get_compute_node(int cpu_cores)
   {
      thread_mutex_lock(&mutex_registered_nodes, NHPC_THREAD_LOCK_WRITE);
      
      int i = 1;
      grid_node_t         *node;
      grid_node_compute_t *compute_node;
      
      while((node = (grid_node_t *)(*registered_nodes).search_inorder_str(i, NULL)) != NULL)
      {
	 compute_node = grid_node_get_compute_node_data(node);
	 
	 if(grid_compute_node_get_available_cores(compute_node) >= cpu_cores)
	 {
	    grid_compute_node_alloc_available_cores(compute_node, cpu_cores);
	    break;
	 }
	 
	 i++;
      }
      
      thread_mutex_unlock(&mutex_registered_nodes, NHPC_THREAD_LOCK_WRITE);
      
      return node;
   }
   
   grid_node_t *grid_node_search_compute_node(const char *node_uid)
   {
      grid_node_t *node;
      
      thread_mutex_lock(&mutex_registered_nodes, NHPC_THREAD_LOCK_READ);      
      node = (grid_node_t *)(*registered_nodes).search(node_uid);
      thread_mutex_unlock(&mutex_registered_nodes, NHPC_THREAD_LOCK_READ);
      
      return node;
   }
   
   void grid_node_free_compute_node(grid_node_t *node, int cpu_cores)
   {
      grid_node_compute_t *compute_node = grid_node_get_compute_node_data(node);
      
      thread_mutex_lock(&mutex_registered_nodes, NHPC_THREAD_LOCK_WRITE);
      grid_compute_node_free_available_cores(compute_node, cpu_cores);
      thread_mutex_unlock(&mutex_registered_nodes, NHPC_THREAD_LOCK_WRITE);
   }
   
   void grid_node_delete_compute_node(grid_node_t *node)
   {
      rbtree       *node_db;
      nhpc_mutex_t *mutex;

      if(grid_node_is_type_compute(node->node_type))
      {
	 node_db = registered_nodes;
	 mutex   = &(mutex_registered_nodes);
      }
      else 
      {
	 node_db = registered_clients;
	 mutex   = &(mutex_registered_clients);
      }
      
      thread_mutex_lock(mutex, NHPC_THREAD_LOCK_WRITE);
      (*node_db).erase(node->peer_uid);
      thread_mutex_unlock(mutex, NHPC_THREAD_LOCK_WRITE);
      
      if(grid_node_is_type_compute(node->node_type))
	 delete ((grid_node_compute_t *)(node->node_data));
      else 
	 delete ((grid_node_client_t *)(node->node_data));
      
      delete[] (node->peer_addr);
      delete[] (node->peer_port);
      delete[] (node->peer_uid);
      delete node;
   }
};