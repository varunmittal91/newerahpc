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

#include <iostream>

#include <include/grid.h>
//#include <include/grid_scheduler.h>
//#include <include/grid_data.h>

using namespace std;

namespace neweraHPC
{	
   grid_scheduler_t::grid_scheduler_t()
   {
      node_count = 5;
      client_count = 5;
      q = new queue_t[node_count];
      tree = new rbtree_t[node_count];
      clientList = new struct peer_details_t[client_count];
   }
   
   grid_scheduler_t::grid_scheduler_t(int nodes)
   {
      node_count = nodes;
      client_count = nodes;
      q = new queue_t[node_count];
      tree = new rbtree_t[node_count];
      clientList = new struct peer_details_t[client_count];
   }
   
   bool grid_scheduler_t::enqueue(int value, int id)
   {
      int status = -1;
      if(q[id].front == q[id].rear && q[id].front == -1)
      {
         q[id].front = q[id].rear = 1;
      }
      else
      {
         q[id].rear++;
      }
      q[id].task_total++;
      status = tree[id].insert(&value, q[id].rear);
      if(status > -1)
         return true;
      return false;
   }
   
   int grid_scheduler_t::dequeue(int id)
   {
      int *value, status = -1;
      if(q[id].front == q[id].rear && q[id].front == -1)
      {
         return -9999;
      }
      else
      {
         value = (int*)tree[id].search(q[id].front);
         status = tree[id].erase(q[id].front);
         if(status != 1)
            return -9999;
         q[id].front++;
         q[id].task_completed++;
         return *value;
      }
   }
   
   int grid_scheduler_t::find_min(int length)
   {
      int min = tree[0].ret_count();
      int pos = 0;
      int i;
      for(i=1; i<length; i++)
         if(tree[i].ret_count() < min)
         {
	    min = tree[i].ret_count();
	    pos = i;
         }
      return pos;
   }
};
