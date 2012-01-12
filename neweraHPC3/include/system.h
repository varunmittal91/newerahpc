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
   class nhpc_system_t
   {
   private:
      thread_manager_t **thread_manager;
      
   public:
      nhpc_system_t();
      nhpc_system_t(thread_manager_t **_thread_manager);
      ~nhpc_system_t();
      void init_system();
      
      static void *monitor_system(nhpc_system_t *system);
   };
};

#endif