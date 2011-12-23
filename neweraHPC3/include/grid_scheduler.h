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

#ifndef _GRID_SCHEDULER_H_
#define _GRID_SCHEDULER_H_

#include "grid_data.h"
#include "rbtree.h"

namespace neweraHPC
{
   typedef struct queue_t{
      int peer_id; 
      int front;
      int rear;
      int task_total;
      int task_completed;
   }queue_t;
   
   class grid_scheduler_t
   {
   private:
      int nodes;
      int node_count;
      int client_count;
      queue_t *q;
      rbtree_t *tree;
      struct peer_details_t *clientList;
      bool enqueue(int, int);
      int dequeue(int);
      bool isIdle(int);
      int find_min(int);
      bool send(peer_details_t *client, plugin_details_t *task);
      bool get(peer_details_t *client);
      void dispatcher(struct peer_details_t *client);
      
   public:
      grid_scheduler_t();
      grid_scheduler_t(int nodes);
      bool addClient(struct peer_details_t *client);
      bool removeClient(struct peer_details_t *client);
      struct peer_details_t* addTask(struct plugin_details_t *task);
      bool dispatch();
   };
};

#endif
