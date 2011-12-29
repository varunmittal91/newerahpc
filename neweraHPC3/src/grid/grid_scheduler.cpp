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
#include <include/network.h>
#include <include/grid_client.h>

using namespace std;

namespace neweraHPC
{	
   grid_scheduler_t::grid_scheduler_t(thread_manager_t **_thread_manager)
   {
      peers = new rbtree_t;
      jobs = new rbtree_t(NHPC_RBTREE_STR);
      queued_instructions = new rbtree_t;
      
      mutex = new pthread_mutex_t;
      pthread_mutex_init(mutex, NULL);
      
      thread_manager = _thread_manager;
   }
   
   grid_scheduler_t::~grid_scheduler_t()
   {
      
   }
   
   void grid_scheduler_t::add_peer(const char *host, const char *port, int processors)
   {
      peer_details_t *peer_details = new peer_details_t;
      nhpc_strcpy(&(peer_details->host), host);
      nhpc_strcpy(&(peer_details->port), port);
      peer_details->processors = processors;
      peer_details->weight = 0;
      
      peer_details->id = peers->insert(peer_details);
   }
   
   peer_details_t *grid_scheduler_t::schedule()
   {
      peer_details_t *peer_details = NULL;
      
      for(int i = 1; i <= peers->ret_count(); i++)
      {
	 peer_details = (peer_details_t *)peers->search(i);	 
	 
	 cout<<peer_details->host<<" "<<peer_details->weight<<" "<<peer_details->processors<<endl;
	 
	 if(peer_details->weight < peer_details->processors)
	 {
	    lock();
	    peer_details->weight++;
	    unlock();
	    return peer_details;
	 }
      }
      
      return NULL;
   }
   
   nhpc_status_t grid_scheduler_t::queue_job(nhpc_instruction_set_t *instruction_set)
   {
      peer_details_t *peer_details;
      char *host_grid_uid = instruction_set->host_grid_uid;
      
      if(!(instruction_set->host_grid_uid))
      {
	 return NHPC_FAIL;
      }
      
      peer_details = schedule();
      if(!peer_details)
      {
	 queued_instructions->insert(instruction_set);
	 
	 return NHPC_FAIL;
      }
	 
      const char *grid_uid;
      nhpc_status_t nrv = nhpc_register_to_server(&grid_uid, peer_details->host, peer_details->port);
      if(nrv != NHPC_SUCCESS)
      {
	 free_peer(peer_details->id);
	 
	 return NHPC_FAIL;
      }
      
      scheduler_thread_data_t *data = new scheduler_thread_data_t;
      data->peer_details = peer_details;
      nhpc_strcpy((char **)&(data->host_grid_uid), host_grid_uid);
      data->instruction_set = instruction_set;
      data->grid_uid = grid_uid;
      
      (*thread_manager)->create_thread(NULL, (void* (*)(void*))scheduler_thread, data, NHPC_THREAD_DEFAULT);      
   }
   
   void grid_scheduler_t::free_peer(int id)
   {
      lock();
      
      peer_details_t *peer_details = (peer_details_t *)peers->search(id);
      if(peer_details)
      {
	 peer_details->weight--;
      }
      
      unlock();
      
      push_jobs();
   }
   
   void grid_scheduler_t::lock()
   {
      pthread_mutex_lock(mutex);
   }
   
   void grid_scheduler_t::unlock()
   {
      pthread_mutex_unlock(mutex);      
   }
   
   nhpc_status_t grid_scheduler_t::push_jobs()
   {
      nhpc_status_t nrv = NHPC_FAIL;
      
      int id;
      
      lock();      
      nhpc_instruction_set_t *instruction_set = (nhpc_instruction_set_t *)queued_instructions->search_first(&id);
      unlock();

      if(instruction_set && (instruction_set->host_grid_uid))
      {
	 nrv = queue_job(instruction_set);
	 lock();
	 queued_instructions->erase(id);
	 unlock();
      }
      
      return nrv;
   }
   
   void *grid_scheduler_t::scheduler_thread(scheduler_thread_data_t *data)
   {
      peer_details_t *peer_details = data->peer_details;
      const char *host_grid_uid = data->host_grid_uid;
      nhpc_instruction_set_t *instruction_set = data->instruction_set;      
      
      const char *host_addr = (const char *)peer_details->host;
      const char *host_port = (const char *)peer_details->port;
      const char *grid_uid = (const char *)data->grid_uid;
      
      nhpc_status_t nrv;
      
      const char *base_dir = nhpc_strconcat("/www/grid/", host_grid_uid, "/");
      
      for(int i = 1; i < instruction_set->arguments->ret_count(); i++)
      {
	 char *argument = (char *)instruction_set->arguments->search(i);
	 char *search_value = nhpc_strconcat(nhpc_itostr(GRID_FILE), "*");
	 if(nhpc_strcmp(argument, search_value) == NHPC_SUCCESS)
	 {
	    string_t *string = nhpc_substr(argument, ',');
	    char *file_path = nhpc_strconcat(base_dir, string->strings[1]);
	    
	    nrv = nhpc_send_file(grid_uid, host_addr, host_port, file_path);
	    
	    delete[] file_path;
	    nhpc_string_delete(string);
	    
	    if(nrv != NHPC_SUCCESS)
	    {  
	       return NHPC_FAIL;
	    }
	 }
	 
	 delete[] search_value;
      }
      
      char *peer_id = nhpc_strconcat("Peer: ", nhpc_itostr(peer_details->id));
      char *host_uid = nhpc_strconcat("Host-Grid-Uid: ", host_grid_uid);
      nrv = nhpc_send_instruction(grid_uid, host_addr, host_port, instruction_set,
				  "Execution-State: Ready", peer_id, host_uid);
      
      if(nrv != NHPC_SUCCESS)
      {
	 return NHPC_FAIL;
      }
      
      delete[] base_dir;
      delete[] peer_id;
      
      delete data;
   }
};
