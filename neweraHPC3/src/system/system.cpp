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

#include <iostream>

#include <include/system.h>

using namespace std;

namespace neweraHPC
{
   nhpc_system_t::nhpc_system_t()
   {
      *thread_manager = new thread_manager_t;
   }
   
   nhpc_system_t::nhpc_system_t(thread_manager_t **_thread_manager)
   {
      thread_manager = _thread_manager;
   }
   
   nhpc_system_t::~nhpc_system_t()
   {
      
   }
   
   void nhpc_system_t::init_system()
   {
      int thread_id;
      (*thread_manager)->init_thread(&thread_id, NULL);
      (*thread_manager)->create_thread(&thread_id, NULL, (void* (*)(void*))monitor_system, this, NHPC_THREAD_DEFAULT);
   }
   
   void *nhpc_system_t::monitor_system(nhpc_system_t *system)
   {
      int bytes_free;
      int bytes_used;
      
      while(1)
      {
	 system_mem_free(&bytes_free);
	 system_mem_used(&bytes_used);
	 
	 cout<<"Memory free: "<<bytes_free<<" Memory Used: "<<bytes_used<<endl;
	 sleep(1);
      }
   }
};
