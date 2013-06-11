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

#ifndef _GRID_NODE_H_
#define _GRID_NODE_H_

#include <iostream>

#include <neweraHPC/thread.h>
#include <neweraHPC/strings.h>

#include "grid_instruction_data_type.h"

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
      int      free_cores;
   };
#define grid_compute_node_get_cores(cn)                 (cn->logical_cores)
#define grid_compute_node_get_free_cores(cn)            (cn->free_cores)
#define grid_compute_node_get_available_cores(cn)       (grid_compute_node_get_free_cores(cn))
#define grid_compute_node_alloc_available_cores(cn, n)  (grid_compute_node_get_free_cores(cn) -= n)
#define grid_compute_node_free_available_cores(cn, n)   (grid_compute_node_get_free_cores(cn) += n)

   static void grid_node_compute_init(grid_node_compute_t **grid_node_compute)
   {
      (*grid_node_compute) = new grid_node_compute_t;
      memset((*grid_node_compute), 0, sizeof(grid_node_compute_t));
   }
   static void grid_node_compute_destruct(grid_node_compute_t *compute_node)
   {
      delete compute_node;
   }
   static void grid_node_compute_set_logical_cores(grid_node_compute_t *grid_node_compute, const char *cpu_cores_str)
   {
      grid_node_compute->logical_cores = nhpc_strtoi(cpu_cores_str);
      grid_node_compute->free_cores    = grid_node_compute->logical_cores;
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
      rbtree       *jobs;
      nhpc_mutex_t  mutex;
   };
   static void grid_node_client_init(grid_node_client_t **grid_node_client)
   {
      (*grid_node_client) = new grid_node_client_t;
      memset((*grid_node_client), 0, sizeof(grid_node_client_t));
      
      (*grid_node_client)->jobs = new rbtree(RBTREE_NUM_MANAGED);
      thread_mutex_init(&((*grid_node_client)->mutex));
   }
   static void grid_node_client_destruct(grid_node_client_t *grid_node_client)
   {
      delete (grid_node_client->jobs);
      delete grid_node_client;
   }
   nhpc_status_t grid_node_client_queue_job(const char *node_uid, grid_instruction_t *instruction);
   
   struct grid_node_t
   {
      const char  *peer_addr;
      const char  *peer_port;
      const char  *peer_uid;
      node_type_t  node_type;
      void        *node_data;
   };
#define grid_node_get_peer_addr(n)  (n->peer_addr)
#define grid_node_get_peer_port(n)  (n->peer_port)
   
   extern rbtree *registered_nodes;
   extern rbtree *registered_clients;
   extern nhpc_mutex_t mutex_registered_nodes;
   extern nhpc_mutex_t mutex_registered_clients;
   
   static void grid_node_db_init()
   {
      registered_nodes   = new rbtree(RBTREE_STR);
      registered_clients = new rbtree(RBTREE_STR);
      
      thread_mutex_init(&mutex_registered_nodes);
      thread_mutex_init(&mutex_registered_clients);
   }
   static void grid_node_init(grid_node_t **grid_node, node_type_t node_type)
   {
      (*grid_node) = new grid_node_t;
      memset((*grid_node), 0, sizeof(grid_node_t));
      grid_node_set_type((*grid_node)->node_type, node_type);
   }
   static void grid_node_destruct(grid_node_t *node)
   {
      if(grid_node_is_type_compute(node->node_type))
	 grid_node_compute_destruct(((grid_node_compute_t *)(node->node_data)));
      else if(grid_node_is_type_client(node->node_type) && (node->node_data)) 
	 delete ((grid_node_client_t *)(node->node_data));
      
      delete[] (node->peer_addr);
      delete[] (node->peer_port);
      delete[] (node->peer_uid);
      delete node;      
   }
   inline void grid_node_set_peer_details(grid_node_t *grid_node, const char *peer_addr, const char *peer_port)
   {
      nhpc_strcpy((char **)&(grid_node->peer_addr), peer_addr);
      nhpc_strcpy((char **)&(grid_node->peer_port), peer_port);
   }
   grid_node_t *grid_node_search_node(const char *node_uid, node_type_t type);

   grid_node_t *grid_node_get_compute_node(int cpu_cores);
   grid_node_t *grid_node_search_compute_node(const char *node_uid);
   void grid_node_free_compute_node(grid_node_t *node, int cpu_cores);
   void grid_node_delete_compute_node(grid_node_t *node);
   void grid_node_delete_client_node(const char *grid_uid);
#define grid_node_get_compute_node_data(n)    (grid_node_compute_t *)(n->node_data)
#define grid_node_get_client_node_data(n)     (grid_node_client_t *)(n->node_data)
#define grid_node_set_compute_node_data(n, d) (n->node_data = (void *)d)
#define grid_node_set_client_node_data(n, d)  (n->node_data = (void *)d)
#define grid_node_set_uid(g, u)               (g->peer_uid = u)
};

#endif