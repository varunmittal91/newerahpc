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
      thread_mutex_lock(&mutex_registered_nodes, NHPC_THREAD_LOCK_WRITE);
      (*registered_nodes).erase(node->peer_uid);
      thread_mutex_unlock(&mutex_registered_nodes, NHPC_THREAD_LOCK_WRITE);

      grid_node_destruct(node);
   }
   
   void grid_node_delete_client_node(const char *grid_uid)
   {
      grid_node_t *node;
      
      thread_mutex_lock(&mutex_registered_clients, NHPC_THREAD_LOCK_WRITE);
      node = (grid_node_t *)(*registered_clients).search(grid_uid);
      if(node)
	 (*registered_nodes).erase(grid_uid);
      thread_mutex_unlock(&mutex_registered_clients, NHPC_THREAD_LOCK_WRITE);
      
      if(node)
	 grid_node_destruct(node);
   }
};