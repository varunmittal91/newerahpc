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
   grid_scheduler_t::grid_scheduler_t()
   {
      peers = new rbtree_t;
      jobs = new rbtree_t(NHPC_RBTREE_STR);
      
      mutex = new pthread_mutex_t;
      pthread_mutex_init(mutex, NULL);
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
      
      peer_details->id = peers->insert(peer_details);
   }
   
   peer_details_t *grid_scheduler_t::schedule()
   {
      peer_details_t *peer_details = NULL;
      
      for(int i = 1; i <= peers->ret_count(); i++)
      {
	 peer_details = (peer_details_t *)peers->search(i);
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
   
   nhpc_status_t grid_scheduler_t::queue_job(nhpc_instruction_set_t *instruction_set, const char *host_grid_uid)
   {
      peer_details_t *peer_details;
      
      do
      {
	 peer_details = schedule();
	 if(!peer_details)
	    sleep(1);
      }while(peer_details == NULL);
      
      const char *host_addr = (const char *)peer_details->host;
      const char *host_port = (const char *)peer_details->port;
      const char *grid_uid;
      
      nhpc_status_t nrv;
      
      nrv = nhpc_register_to_server(&grid_uid, host_addr, host_port);
      if(nrv != NHPC_SUCCESS)
	 return NHPC_FAIL;
      
      cout<<"Reserved Grid-Uid: "<<grid_uid<<endl;
      const char *base_dir = nhpc_strconcat("/www/grid/", host_grid_uid, "/");
      
      for(int i = 1; i < instruction_set->arguments->ret_count(); i++)
      {
	 char *argument = (char *)instruction_set->arguments->search(i);
	 char *search_value = nhpc_strconcat(nhpc_itostr(GRID_FILE), "*");
	 if(nhpc_strcmp(argument, search_value) == NHPC_SUCCESS)
	 {
	    cout<<base_dir<<endl;
	    string_t *string = nhpc_substr(argument, ',');
	    char *file_path = nhpc_strconcat(base_dir, string->strings[1]);
	    cout<<file_path<<endl;
	    
	    nrv = nhpc_send_file(grid_uid, host_addr, host_port, file_path);
	    
	    delete[] file_path;
	    nhpc_string_delete(string);
	    
	    if(nrv != NHPC_SUCCESS)
	       return NHPC_FAIL;
	 }

	 delete[] search_value;
      }
      
      char *peer_id = nhpc_strconcat("Peer: ", nhpc_itostr(peer_details->id));
      nrv = nhpc_send_instruction(grid_uid, host_addr, host_port, instruction_set,
				  "Execution-State: Ready", peer_id);
      
      if(nrv != NHPC_SUCCESS)
	 return NHPC_FAIL;
      
      cout<<"done"<<endl;
      delete[] base_dir;
      delete[] peer_id;
      
      return NHPC_SUCCESS;
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
   }
   
   void grid_scheduler_t::lock()
   {
      pthread_mutex_lock(mutex);
   }
   
   void grid_scheduler_t::unlock()
   {
      pthread_mutex_unlock(mutex);      
   }
};
