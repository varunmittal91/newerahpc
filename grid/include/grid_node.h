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

#ifndef _GRID_CLIENT_H_
#define _GRID_CLIENT_H_

#include <iostream>
using namespace std;

namespace neweraHPC 
{
#define NODE_TYPE_COMPUTE 1
#define NODE_TYPE_CLIENT  2
   typedef unsigned char node_type_t;
#define grid_node_set_type(n, t)      (n = t)
#define grid_node_set_type_compute(n) (n, NODE_TYPE_COMPUTE)
#define grid_node_set_type_client(n)  (n, NODE_TYPE_CLIENT)
#define grid_node_is_type_compute(n)  (n & 1)
#define grid_node_is_type_client(n)   ((n >> 1) & 1)
   
   struct grid_node_compute_t
   {
      int      logical_cores;
      long int total_mem;
      long int free_mem;
   };
   static void grid_node_compute_init(grid_node_compute_t **grid_node_compute)
   {
      (*grid_node_compute) = new grid_node_compute_t;
      memset((*grid_node_compute), 0, sizeof(grid_node_compute_t));
   }
   static void grid_node_compute_set_logical_cores(grid_node_compute_t *grid_node_compute, const char *cpu_cores_str)
   {
      grid_node_compute->logical_cores = nhpc_strtoi(cpu_cores_str);
   }
   static void grid_node_compute_set_total_mem(grid_node_compute_t *grid_node_compute, const char *total_mem)
   {
      grid_node_compute->total_mem = nhpc_strtoi(total_mem);
   }
   static void grid_node_compute_set_free_mem(grid_node_compute_t *grid_node_compute, const char *free_mem)
   {
      grid_node_compute->free_mem = nhpc_strtoi(free_mem);      
   }
   
   struct grid_node_client_t
   {
      rbtree *jobs;
   };
   
   struct grid_node_t
   {
      const char  *peer_addr;
      const char  *peer_port;
      const char  *peer_uid;
      node_type_t  node_type;
      void        *node_data;
   };
   
   extern rbtree *registered_nodes;
   extern rbtree *registered_clients;
   
   static void grid_node_db_init()
   {
      registered_nodes   = new rbtree(RBTREE_STR);
      registered_clients = new rbtree(RBTREE_STR);
   }
   static void grid_node_init(grid_node_t **grid_node, node_type_t node_type)
   {
      (*grid_node) = new grid_node_t;
      memset((*grid_node), 0, sizeof(grid_node_t));
      grid_node_set_type((*grid_node)->node_type, node_type);
   }
   static void grid_node_set_peer_details(grid_node_t *grid_node, const char *peer_addr, const char *peer_port)
   {
      nhpc_strcpy((char **)&(grid_node->peer_addr), peer_addr);
      nhpc_strcpy((char **)&(grid_node->peer_port), peer_port);
   }
   static void grid_node_set_uid(grid_node_t *grid_node, const char *uid)
   {
      nhpc_strcpy((char **)&(grid_node->peer_uid), uid);
   }
#define grid_node_get_compute_node_data(n)    (grid_node_compute_t *)(n->node_data)
#define grid_node_get_client_node_data(n)     (grid_node_client_t *)(n->node_data)
#define grid_node_set_compute_node_data(n, d) (n->node_data = (void *)d)
#define grid_node_set_client_node_data(n, d)  (n->node_data = (void *)d)
};

#endif