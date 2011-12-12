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

#ifndef _GRID_PLUGIN_H_
#define _GRID_PLUGIN_H_

#include <pthread.h>

#include "rbtree.h"
#include "thread.h"

namespace neweraHPC 
{
   class plugin_manager_t
   {
   private:
      rbtree_t *plugins_installed;
      rbtree_t *plugins_requested;
      thread_manager_t *thread_manager;
      pthread_mutex_t *mutex;
      
   public:
      plugin_manager_t(thread_manager_t *in_thread_manager);
      ~plugin_manager_t();
      void lock();
      void unlock();
      int request_plugin(int peer_id, int remote_plugin_id);
   };
   
   void *plugin_request_thread(plugin_manager_t *plugin_manager);
};

#endif