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
#include <sys/types.h>

#include <include/system.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC
{
   nhpc_system_t::nhpc_system_t()
   {
      *thread_manager = new thread_manager_t;
      
      systeminfo = new nhpc_systeminfo_t;
   }
   
   nhpc_system_t::nhpc_system_t(thread_manager_t **_thread_manager)
   {
      thread_manager = _thread_manager;
      
      systeminfo = new nhpc_systeminfo_t;
   }
   
   nhpc_system_t::~nhpc_system_t()
   {
      LOG_INFO("Shuting Down System Manager");
	       
      delete systeminfo;
   }
   
   void nhpc_system_t::init_system()
   {      
      thread_mutex_init(&mutex);
      
      int thread_id;
      (*thread_manager)->init_thread(&thread_id, NULL);
      (*thread_manager)->create_thread(&thread_id, NULL, (void* (*)(void*))monitor_system, this, NHPC_THREAD_DEFAULT);
   }   
   
   nhpc_status_t nhpc_system_t::system_memstats(nhpc_meminfo_t **_meminfo)
   {
      thread_mutex_lock(&mutex, 1);
      
      *_meminfo = new nhpc_meminfo_t;
      memcpy(_meminfo, &(systeminfo->meminfo), sizeof(nhpc_meminfo_t));
      
      thread_mutex_unlock(&mutex, 1);
   }
   
   nhpc_status_t nhpc_system_t::system_cpustats(nhpc_cpuinfo_t **_cpuinfo)
   {
      thread_mutex_lock(&mutex, 1);
      
      *_cpuinfo = new nhpc_cpuinfo_t;
      memcpy(_cpuinfo, &(systeminfo->cpuinfo), sizeof(nhpc_cpuinfo_t));
      
      thread_mutex_unlock(&mutex, 1);
   }
   
   nhpc_status_t nhpc_system_t::system_stats(nhpc_systeminfo_t **_systeminfo)
   {
      thread_mutex_lock(&mutex, 1);
      
      *_systeminfo = new nhpc_systeminfo_t;
      memcpy(_systeminfo, systeminfo, sizeof(nhpc_systeminfo_t));
	     
      thread_mutex_unlock(&mutex, 1);
   }   

   nhpc_status_t system_systeminfo(nhpc_systeminfo_t *systeminfo)
   {
      system_cpuinfo(&(systeminfo->cpuinfo));
      system_meminfo(&(systeminfo->meminfo));
   }
   
   void *nhpc_system_t::monitor_system(nhpc_system_t *system)
   {
      nhpc_systeminfo_t *systeminfo = system->systeminfo;
      
      while(1)
      {	 
	 thread_mutex_lock(&(system->mutex), 0);
	 system_systeminfo(systeminfo);
	 thread_mutex_lock(&(system->mutex), 0);
	 sleep(1);
      }
   }
};
