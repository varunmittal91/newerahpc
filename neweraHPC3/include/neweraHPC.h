/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
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

#ifndef _NEWERAHPC_H_
#define _NEWERAHPC_H_

#include "rbtree.h"
#include "thread.h"
#include "network.h"
#include "strings.h"

namespace neweraHPC
{
   struct worker_threads
   {
      int connection_id;
   };
   
   class neweraHPC_main
   {
   private:
      network_t *main_network;
      rbtree_t *worker_threads;
   public:
      neweraHPC_main();
      ~neweraHPC_main();
   };  
};

#endif