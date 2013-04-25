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

#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <sys/sysctl.h>
#else
#include <fstream>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/alloc.h>
#include <include/system.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC
{
   nhpc_sysinfo_t nhpc_sysinfo;
   
   void nhpc_system_update_cpuinfo()
   {
      nhpc_sysinfo.logical_cores = sysconf(_SC_NPROCESSORS_CONF);
      
      getloadavg(&(nhpc_sysinfo.load_avg_5), 3);
      
      double max = nhpc_sysinfo.load_avg_5;
      if(max < nhpc_sysinfo.load_avg_15)
	 max = nhpc_sysinfo.load_avg_15;
      if(max < nhpc_sysinfo.load_avg_25)
	 max = nhpc_sysinfo.load_avg_25;
      
      nhpc_sysinfo.load_avg_max = max;      
   }
   
   void nhpc_system_update_meminfo()
   {
#ifdef __APPLE__
      int mib[2];
      mib[0] = CTL_HW;
      mib[1] = HW_MEMSIZE;
      size_t length = sizeof(long int);

      sysctl(mib, 2, &(nhpc_sysinfo.total_mem), &length, NULL, 0);   
      
      vm_size_t page_size;
      mach_port_t mach_port;
      mach_msg_type_number_t count;
      vm_statistics_data_t vm_stats;
      
      mach_port = mach_host_self();
      count = sizeof(vm_stats) / sizeof(natural_t);
      
      if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
	  KERN_SUCCESS == host_statistics(mach_port, HOST_VM_INFO, 
					  (host_info_t)&vm_stats, &count))
      {
	 nhpc_sysinfo.free_mem = (int64_t)vm_stats.free_count * (int64_t)page_size;
      }
#else 
      ifstream in;
      char buffer[20];
      in.open("/proc/meminfo");
      while(in.good())
      {
	 in>>buffer;
	 if(strcmp(buffer, "MemTotal:") == 0){
	    in>>buffer;
	    nhpc_sysinfo.total_mem = atoi(buffer);
	 }
	 if(strcmp(buffer, "MemFree:") == 0){
	    in>>buffer;
	    nhpc_sysinfo.free_mem = atoi(buffer);
	 }
	 if(strcmp(buffer, "SwapTotal:") == 0){
	    in>>buffer;
	    nhpc_sysinfo.total_swap = atoi(buffer);
	 }
	 if(strcmp(buffer, "SwapFree:") == 0){
	    in>>buffer;
	    nhpc_sysinfo.free_swap = atoi(buffer);
	 }
      }      
      
      in.close();
#endif
   }
   
   nhpc_system_t *nhpc_system;
   
   void nhpc_system_init()
   {
      nhpc_system = new nhpc_system_t;
      (*nhpc_system).init_system();
   }
   
   void nhpc_system_destroy()
   {
      delete nhpc_system;
   }   
   
   void child_handler(int signum)
   {
      if(signum != SIGCHLD)
	 return;
      
      (*nhpc_system).free_child_process();
   }

   nhpc_system_t::nhpc_system_t()
   {
      thread_manager = new thread_manager_t*;
      *thread_manager = new thread_manager_t;
      
      systeminfo = new nhpc_systeminfo_t;
      
      thread_mutex_init(&mutex_chld);
      
      child_processes = new rbtree(RBTREE_NUM);
      child_handlers  = new rbtree(RBTREE_STR);
   }
   
   nhpc_system_t::nhpc_system_t(thread_manager_t **_thread_manager)
   {
      thread_manager = _thread_manager;
      
      systeminfo = new nhpc_systeminfo_t;

      child_processes = new rbtree(RBTREE_NUM);
      child_handlers  = new rbtree(RBTREE_STR);
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

      pthread_atfork(NULL, NULL, child_prepare);
      
      struct sigaction act;
      memset (&act, 0, sizeof(act));
      act.sa_handler = child_handler;
      
      if (sigaction(SIGCHLD, &act, 0)) {
	 perror ("sigaction");
      }
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
      
      nhpc_cpuinfo_t *temp_cpuinfo = new nhpc_cpuinfo_t;
      cout << temp_cpuinfo << endl;
      
      (*_cpuinfo) = temp_cpuinfo;
      memcpy(_cpuinfo, &(systeminfo->cpuinfo), sizeof(nhpc_cpuinfo_t));
      cout << "test" << endl;
      cout << (*_cpuinfo) << endl;
      
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
	 system->clean_inactive_children();
	 sleep(1);
	 continue;
	 
	 thread_mutex_lock(&(system->mutex), 0);
	 system_systeminfo(systeminfo);
	 thread_mutex_unlock(&(system->mutex), 0);
	 
	 LOG_INFO("System: Max CPU Uage - " << systeminfo->cpuinfo.load_avg_max);
	 LOG_INFO("System: Free Memory  - " << systeminfo->meminfo.free_mem);
	 
	 sleep(1);
      }
   }
};
