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
#include <sys/sysctl.h>
#include <sys/types.h>

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
      nhpc_meminfo meminfo;
      
      int mib[2];
      long int physical_memory;
      size_t length;
      
#ifdef __APPLE__
      mib[0] = CTL_HW;
      mib[1] = HW_MEMSIZE;
      length = sizeof(long int);
      sysctl(mib, 2, &(meminfo.total_mem), &length, NULL, 0);
#endif
            
      while(1)
      {	 
	 system_meminfo(&meminfo);
	 cout<<"Memory free: "<<meminfo.free_mem<<" Memory total: "<<meminfo.total_mem<<endl;
	 sleep(1);
      }
   }
};
