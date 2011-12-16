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

using namespace std;

namespace neweraHPC
{	
   grid_scheduler_t::grid_scheduler_t()
   {
	  node_count = 0;
      nodes = 5;
      client_count = -1;
      q = new queue_t[nodes];
      tree = new rbtree_t[nodes];
      clientList = new struct peer_details_t[nodes];
   }
   
   grid_scheduler_t::grid_scheduler_t(int nodes_)
   {
	  node_count = 0; 
      nodes = nodes_;
      client_count = -1;
      q = new queue_t[nodes];
      tree = new rbtree_t[nodes];
      clientList = new struct peer_details_t[nodes];
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

   bool grid_scheduler_t::addClient(struct peer_details_t *client)
   {
	   client_count++;
       clientList[client_count] = *client;
	   q[node_count].peer_id = client->	id;
	   q[node_count].rear = q[node_count].front = -1;
	   q[node_count].task_total = q[node_count].task_completed = 0;
	   return true;
   }
   
   bool grid_scheduler_t::removeClient(struct peer_details_t *client)
   {
	   if(client_count == -1)
		   return false;
	   else
	   {
		   int i, j, cntr = 0;
		   for(i = 0; i < client_count; i++)
		   {   
			   if(q[cntr].peer_id == client->id)
			   {
				   for(j = i; j < node_count - 1; j++)	   
				      q[j] = q[j+1];
				   node_count--;
			   }
				   
		   }
		   for(i = 0; i < client_count; i++)
			   if(clientList[i].id == client->id)
				   break;
			for(j = i; j < client_count - 1; j++)	   
				clientList[j] = clientList[j+1];
			client_count--;
			
	   }	   
   }
   
   bool grid_scheduler_t::dispatch()
   {
	   pthread_t threads[50];
	   int i;
	   for(i = 0; i < node_count; i++ )
	   {
		   //pthread_create(&thread[i], NULL, dispatcher, )
	   }
   }
};
