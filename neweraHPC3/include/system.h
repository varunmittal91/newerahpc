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

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "system_memory.h"
#include "thread.h"

namespace neweraHPC
{
   struct nhpc_cpuinfo_t
   {
      double load_avg_5;
      double load_avg_15;
      double load_avg_25;
      double load_avg_max;
   };
   
   struct nhpc_systeminfo_t
   {
      nhpc_cpuinfo_t cpuinfo;
      nhpc_meminfo_t meminfo;
   };
   
   class nhpc_system_t
   {
   private:
      thread_manager_t **thread_manager;
      nhpc_systeminfo_t *systeminfo;
      nhpc_mutex_t mutex;
      
   public:
      nhpc_system_t();
      nhpc_system_t(thread_manager_t **_thread_manager);
      ~nhpc_system_t();
      void init_system();
      
      nhpc_status_t system_memstats(nhpc_meminfo_t **meminfo);
      nhpc_status_t system_cpustats(nhpc_cpuinfo_t **cpuinfo);
      nhpc_status_t system_stats(nhpc_systeminfo_t **systeinfo);
      
      static void *monitor_system(nhpc_system_t *system);
   };
   
   double cpu_usage();
   nhpc_status_t system_cpuinfo(nhpc_cpuinfo_t *cpuinfo);
   nhpc_status_t system_systeminfo(nhpc_systeminfo_t *systeminfo);  
   nhpc_status_t system_loadavg(nhpc_cpuinfo_t *cpuinfo);
};

#endif