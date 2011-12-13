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

#ifndef _GRID_DATA_H_
#define _GRID_DATA_H_

#include "grid_data.h"
#define MAX_LVL_1 3

namespace neweraHPC
{
   /* Store Network connection details */
   typedef struct list_node_t
   {
      struct peer_details_t *info;
      struct list_node_t    *next;
      struct list_node_t    *connection;
   }list_node_t;

   class grid_scheduler_t
   {
   private:
      list_node_t *level1;
      list_node_t **lists;
   public:
      void init();
      grid_scheduler_t();
      ~grid_scheduler_t();
      /* Adds a new node to network and schedules it */
      int insert(struct peer_details_t *child);

      /* Adds the child to parent node */
      int insert(struct peer_details_t *parent, struct peer_details_t *child);

      /* Remove a node from network and re-schedule if required */
      int remove(struct peer_details_t *node);
   };

#endif
