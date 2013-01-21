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
#include <iomanip>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/grid.h>
#include <include/network.h>
#include <include/file.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC
{
   plugin_manager_t::plugin_manager_t(thread_manager_t **in_thread_manager)
   {
      thread_manager = in_thread_manager;
      plugins_installed = new rbtree(RBTREE_STR);
      plugins_requested = new rbtree(RBTREE_STR);
      mutex = new pthread_mutex_t;
      nhpc_mutex = new nhpc_mutex_t;

      pthread_mutex_init(mutex, NULL);
      thread_mutex_init(nhpc_mutex);

      grid_directory = nhpc_strconcat(HTTP_ROOT, "/grid/");
   }
   
   plugin_manager_t::~plugin_manager_t()
   {
      LOG_INFO("Shuting down plugin manager");

      int plugin_count = (*plugins_installed).length();
      plugin_details_t *tmp;
      const char       *plugin_dir;
      const char       *plugin_nxi;
      const char       *plugin_dll;
      for(int i = 1; i <= plugin_count; i++)
      {
	 tmp = (plugin_details_t *)(*plugins_installed).search_inorder_str(i, NULL);
	 if((plugin_dir) = tmp->plugin_dir)
	 {
	    plugin_nxi = tmp->path_nxi;
	    plugin_dll = tmp->path_plugin;
	    
	    if(plugin_nxi)
	       nhpc_delete_file_dir(plugin_nxi);
	    if(plugin_dll)
	       nhpc_delete_file_dir(plugin_dll);
	    
	    nhpc_delete_file_dir(plugin_dir);
	 }
      }
      
      delete plugins_requested;
      delete plugins_installed;
      
      delete mutex;
      delete nhpc_mutex;
      nhpc_string_delete(grid_directory);
      
      LOG_INFO("Plugin manager down");
   }
   
   void plugin_manager_t::plugin_manager_init()
   {
      int thread_id;
      (*thread_manager)->init_thread(&thread_id, NULL);
      (*thread_manager)->create_thread(&thread_id, NULL, (void* (*)(void*))nhpc_plugin_request_thread, 
				       this, NHPC_THREAD_DEFAULT);
      
      plugin_details_t *nhpc_plugin;
      nhpc_grid_range_plugin_init(&nhpc_plugin);
      (*plugins_installed).insert(nhpc_plugin, nhpc_plugin->plugin_name);
   }
   
   void plugin_manager_t::lock()
   {
      pthread_mutex_lock(mutex);
   }
   
   void plugin_manager_t::unlock()
   {
      pthread_mutex_unlock(mutex);
   }
   
   nhpc_status_t plugin_manager_t::search_plugin(const char *plugin_name, plugin_details_t **plugin_details)
   {
      thread_mutex_lock(nhpc_mutex, NHPC_THREAD_LOCK_READ);      
      (*plugin_details) = (plugin_details_t *)(*plugins_installed).search((char *)plugin_name);
      thread_mutex_unlock(nhpc_mutex, NHPC_THREAD_LOCK_READ);      
      
      if(!(*plugin_details))
	 return NHPC_FAIL;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t plugin_manager_t::install_plugin(const char *file_path, const char *base_dir)
   {
      const char *dll_path;
      char *dll_path_new;
      bool have_nxi = false;
      
      nhpc_status_t nrv = nhpc_check_nxi(file_path);
      
      if(nrv == NHPC_SUCCESS)
      {
	 nrv = nhpc_nxitodll(&dll_path, file_path);
	 if(nrv != NHPC_SUCCESS)
	 {
	    return nrv;
	 }
	 
	 have_nxi = true;
      }
      else 
	 dll_path = (char *)file_path;
      
      plugin_details_t *new_plugin;
      
      nrv = install_plugin_dll(dll_path, &new_plugin);
      
      if(nrv == NHPC_SUCCESS)
      {
	 if(have_nxi)
	 {
	    nrv = copy_filetogrid(file_path, &base_dir, &dll_path_new);
	    
	    if(nrv == NHPC_FAIL)
	    {
	       lock();
	       plugins_installed->erase(new_plugin->plugin_name);
	       unlock();
	       delete new_plugin;

	       return nrv;
	    }
	    
	    if(dll_path_new != NULL)
	       new_plugin->path_nxi = dll_path_new;
	    else 
	       new_plugin->path_nxi = file_path;
	 }
	 
	 nrv = copy_filetogrid(dll_path, &base_dir, &dll_path_new);
	 
	 if(nrv == NHPC_FAIL)
	 {
	    lock();
	    plugins_installed->erase(new_plugin->plugin_name);
	    unlock();
	    nhpc_string_delete((char *)new_plugin->path_nxi);
	    delete new_plugin;
	    
	    return nrv;
	 }
	 
	 new_plugin->plugin_dir = nhpc_strconcat(grid_directory, base_dir);
	 if(dll_path_new != NULL)
	    new_plugin->path_plugin = dll_path_new;
	 else 
	    new_plugin->path_plugin = file_path;

	 LOG_INFO("New plugin details:");
	 LOG_INFO("\t" << new_plugin->plugin_name);
	 if(new_plugin->path_nxi)
	    LOG_INFO("\t" << new_plugin->path_nxi);
	 LOG_INFO("\t" << new_plugin->path_plugin);
	 LOG_INFO("\t" << new_plugin->plugin_dir);
      }
      
      return nrv;
   }
   
   nhpc_status_t plugin_manager_t::install_plugin_dll(const char *dll_path, plugin_details_t **plugin_details)
   {
      void *dll = dlopen(dll_path, RTLD_NOW);
      if(!dll)
      {
	 LOG_ERROR("GRID PLUGIN:" << dlerror());
	 return NHPC_FAIL;
      }
      
      fnc_ptr_nhpc_t  plugin_init_fnc    = (fnc_ptr_nhpc_t)dlsym(dll, "plugin_init");  
      if(!plugin_init_fnc)
      {
	 LOG_ERROR("GRID_PLUGIN:" << dlerror());
	 return NHPC_FAIL;
      }
      
      *plugin_details = new plugin_details_t;
      memset(*plugin_details, 0, sizeof(plugin_details_t));
      
      nhpc_status_t   plugin_init_status = (nhpc_status_t)plugin_init_fnc(plugin_details);
      char           *plugin_name;
      
      if(plugin_init_status != NHPC_SUCCESS)
      {
	 LOG_ERROR("GRID PLUGIN:Plugin initialization failed");
	 return NHPC_FAIL;
      }
      
      if(!(*plugin_details)->fnc_init && !(*plugin_details)->fnc_exec 
	 && !(*plugin_details)->fnc_client_exec && !(*plugin_details)->fnc_processor)
      {
	 LOG_ERROR("GRID_PLUGIN:Plugin symbols not found");
	 delete (*plugin_details);
	 return NHPC_FAIL;
      }
      plugin_name = (*plugin_details)->plugin_name;
      
      void *test_installed = plugins_installed->search(plugin_name);
      {
	 if(test_installed != NULL)
	 {
	    LOG_ERROR("Plugin installation test failed");
	    dlclose(dll);
	    delete (*plugin_details);
	    return NHPC_FAIL;
	 }
      }
      
      lock();
      nhpc_status_t nrv = plugins_installed->insert(*plugin_details, plugin_name);
      unlock();
      if(nrv == NHPC_FAIL)
      {
	 delete (*plugin_details);
	 dlclose(dll);
	 return NHPC_FAIL;
      }
      
      nhpc_strcpy(&((*plugin_details)->plugin_name), plugin_name);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t plugin_manager_t::copy_filetogrid(const char *file_path, const char **base_dir, char **file_path_new)
   {  
      *file_path_new = NULL;
      
      char *search_path = nhpc_strconcat(grid_directory, "*");
      
      if(nhpc_strcmp(file_path, search_path) == NHPC_FAIL)
      {
	 string_t *string = nhpc_substr(file_path, '/');
	 
	 char *grid_dir;
	 char *mkdir_path;
	 
	 if(*base_dir == NULL)
	 {
	    *base_dir = nhpc_random_string(7);
	    mkdir_path = nhpc_strconcat(grid_directory, *base_dir);
	    
	    if(mkdir(mkdir_path, 0777) == -1)
	    {
	       nhpc_string_delete(mkdir_path);
	       nhpc_string_delete((char *)*base_dir);
	       
	       nhpc_string_delete(string);
	       return NHPC_FAIL;
	    }
	    
	    grid_dir = mkdir_path;
	 }
	 else 
	    grid_dir = nhpc_strconcat(grid_directory, *base_dir);
	 
	 
	 char *new_path = nhpc_strconcat(grid_dir, "/");
	 nhpc_string_delete(grid_dir);
	 grid_dir = new_path;
	 new_path = nhpc_strconcat(new_path, string->strings[string->count - 1]);
	 nhpc_string_delete(grid_dir);
	 
	 nhpc_string_delete(string);
	 
	 nhpc_status_t nrv = nhpc_filecopy(new_path, file_path);

	 if(nrv != NHPC_SUCCESS)
	 {
	    return NHPC_FAIL;
	 }
	 
	 *file_path_new = new_path;
      }
      
      nhpc_string_delete(search_path);
            
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t plugin_manager_t::request_plugin(int peer_id, char *plugin_name)
   {
      plugin_request_t *plugin_request;

      thread_mutex_lock(nhpc_mutex, NHPC_THREAD_LOCK_READ);      
      plugin_request = (plugin_request_t *)plugins_requested->search(plugin_name);
      thread_mutex_unlock(nhpc_mutex, NHPC_THREAD_LOCK_READ);

      if(plugin_request)
      {
         while(plugin_request->status == PLUGIN_UNDER_PROCESSING)
         {
            sleep(2);
         } 

         if(plugin_request->status == PLUGIN_INSTALLED)
            return NHPC_SUCCESS;
         else
            return NHPC_FAIL;
      }
      else
      {
         plugin_request = new plugin_request_t;
         plugin_request->peer_id = peer_id;
         thread_mutex_lock(nhpc_mutex, NHPC_THREAD_LOCK_WRITE);      
         plugins_requested->insert(plugin_request, plugin_name);
         thread_mutex_lock(nhpc_mutex, NHPC_THREAD_LOCK_WRITE);       
      }
   }

   nhpc_status_t plugin_manager_t::recieve_plugin(const char *plugin_name, const char *plugin_type, 
						  const char *file_path)
   {

   }


   void *nhpc_plugin_request_thread(plugin_manager_t *plugin_manager)
   {
      
   }
};
