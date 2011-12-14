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
   plugin_manager_t::plugin_manager_t(thread_manager_t *in_thread_manager)
   {
      thread_manager = in_thread_manager;
      plugins_installed = new rbtree_t;
      plugins_requested = new rbtree_t;
      mutex = new pthread_mutex_t;
      pthread_mutex_init(mutex, NULL);
      grid_directory = nhpc_strconcat(HTTP_ROOT, "/grid/");
      
      thread_manager->create_thread(NULL, (void* (*)(void*))nhpc_plugin_request_thread, this, NHPC_THREAD_DEFAULT);
   }
   
   plugin_manager_t::~plugin_manager_t()
   {
      
   }
   
   void plugin_manager_t::lock()
   {
      pthread_mutex_lock(mutex);
   }
   
   void plugin_manager_t::unlock()
   {
      pthread_mutex_unlock(mutex);
   }
   
   nhpc_status_t plugin_manager_t::install_plugin(const char *file_path)
   {
      nhpc_status_t nrv = nhpc_check_nxi(file_path);
      char *dll_path;
      
      if(nrv == NHPC_SUCCESS)
      {
	 dll_path = nhpc_nxitodll(file_path);
	 if(dll_path != NULL)
	 {
	    dll_path = copy_filetogrid(dll_path);
	 }
      }
      else 
	 dll_path = (char *)file_path;
      
      nrv = install_plugin_dll(dll_path);
      if(nrv == NHPC_SUCCESS)
      {
	 dll_path = copy_filetogrid(dll_path);
      }
      
      return nrv;
   }
   
   nhpc_status_t plugin_manager_t::install_plugin_dll(const char *dll_path)
   {
      
   }
   
   char *plugin_manager_t::copy_filetogrid(const char *file_path)
   {
      
   }
   
   void *nhpc_plugin_request_thread(plugin_manager_t *plugin_manager)
   {
      
   }
};