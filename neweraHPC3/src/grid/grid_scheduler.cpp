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
   grid_scheduler_t *scheduler;
   
   grid_scheduler_t::grid_scheduler_t(thread_manager_t **_thread_manager)
   {
      peers = new rbtree_t;
      jobs = new rbtree_t(NHPC_RBTREE_STR);
      queued_instructions = new rbtree_t;
      child_processes = new rbtree_t;
      
      mutex = new pthread_mutex_t;
      mutex_child_processes = new pthread_mutex_t;
      pthread_mutex_init(mutex, NULL);
      pthread_mutex_init(mutex_child_processes, NULL);      
      
      thread_manager = _thread_manager;
      scheduler = this;
   }
   
   grid_scheduler_t::~grid_scheduler_t()
   {
      
   }
   
   void grid_scheduler_t::grid_scheduler_init()
   {
      int thread_id;
      (*thread_manager)->init_thread(&thread_id, NULL);
      (*thread_manager)->create_thread(&thread_id, NULL, (void* (*)(void*))grid_scheduler_t::monitor_jobs_pending, 
				       this, NHPC_THREAD_DEFAULT);
      signal(SIGCHLD, grid_scheduler_t::child_handler);
   }   
   
   int grid_scheduler_t::cores()
   {
      lock();
      int count = peers->ret_count();
      int cores = 0;
      for(int i = 1; i <= count; i++)
      {
         peer_details_t *peer_details = (peer_details_t *)(*peers)[i];
         cores+= (peer_details->processors - peer_details->weight);
      }
      unlock();

      return cores;
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
   
   void grid_scheduler_t::remove_peer(int peer_id)
   {
      lock();
      peer_details_t* peer_details = (peer_details_t *)peers->search(peer_id);
      peers->erase(peer_id);
      unlock();
      if(peer_details)
      {
	 delete[] peer_details->host;
	 delete[] peer_details->port;
	 delete peer_details;
      }
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
   
   nhpc_status_t grid_scheduler_t::queue_job(nhpc_instruction_set_t *instruction_set)
   {
      peer_details_t *peer_details;
      const char *host_addr;
      const char *host_port;
      const char *grid_uid;
      char *host_grid_uid = instruction_set->host_grid_uid;
      
      if(!(instruction_set->host_grid_uid))
      {	 
	 return NHPC_FAIL;
      }
      
      peer_details = schedule();
      if(!peer_details)
      {
	 lock();
	 queued_instructions->insert(instruction_set);
	 unlock();
	 
	 return NHPC_FAIL;
      }
      
      host_addr = (const char *)peer_details->host;
      host_port = (const char *)peer_details->port;
	 
      nhpc_status_t nrv = nhpc_register_to_server(&grid_uid, peer_details->host, peer_details->port);
      if(nrv != NHPC_SUCCESS)
      {
	 free_peer(peer_details->id);
	 lock();
	 queued_instructions->insert(instruction_set);
	 unlock();
	 
	 if(nrv == ECONNREFUSED)
	 {
	    remove_peer(peer_details->id);
	 }
         else 
         {
            perror("");
            exit(1);
         }
	 
	 return NHPC_FAIL;
      }

      const char *base_dir = nhpc_strconcat("/www/grid/", host_grid_uid, "/");
      
      for(int i = 1; i < instruction_set->arguments->ret_count(); i++)
      {
	 char *argument = (char *)instruction_set->arguments->search(i);
	 char *grid_file_str = nhpc_itostr(GRID_FILE);
	 char *search_value = nhpc_strconcat(grid_file_str, "*");
	 delete[] grid_file_str;
	 if(nhpc_strcmp(argument, search_value) == NHPC_SUCCESS)
	 {
	    string_t *string = nhpc_substr(argument, ',');
	    char *file_path = nhpc_strconcat(base_dir, string->strings[1]);
	    delete[] base_dir;
	    
	    nrv = nhpc_send_file(grid_uid, host_addr, host_port, file_path);
	    
	    delete[] file_path;
	    nhpc_string_delete(string);
	    
	    if(nrv != NHPC_SUCCESS)
	    {  
	       free_peer(peer_details->id);
	       lock();
	       queued_instructions->insert(instruction_set);
	       unlock();
	       
	       return NHPC_FAIL;
	    }
	 }
	 
	 delete[] search_value;
      }
            
      instruction_set->host_peer_id = peer_details->id;
      
      nrv = nhpc_send_general_instruction(instruction_set);
      
      if(nrv != NHPC_SUCCESS)
      {
	 instruction_set->host_peer_id = 0;
	 lock();
	 queued_instructions->insert(instruction_set);
	 unlock();	 
	 
	 return NHPC_FAIL;
      }
      
      delete[] grid_uid;
      
      nhpc_delete_instruction(instruction_set);
      
      return nrv;
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
	 lock();
	 queued_instructions->erase(id);
	 unlock();
	 nrv = queue_job(instruction_set);
      }
      
      return nrv;
   }
   
   nhpc_status_t grid_scheduler_t::add_child_process(nhpc_instruction_set_t *instruction_set, pid_t *pid)
   {
      pthread_mutex_lock(mutex_child_processes);
      (*child_processes).insert(instruction_set, *pid);
      pthread_mutex_unlock(mutex_child_processes);
   }
   
   nhpc_status_t grid_scheduler_t::free_child_process()
   {
      pid_t pid;
      int status;
      
      while(1)
      {
	 pid = waitpid(-1, &status, WUNTRACED);
	 if(pid == -1)
	    break;
	 
	 nhpc_instruction_set_t *instruction_set = (nhpc_instruction_set_t *)(*child_processes).search(pid);
	 if(!instruction_set)
	    continue;
	 
	 if(status != 0)
	 {
	    queue_job(instruction_set);
	    continue;
	 }
	 
	 nhpc_status_t nrv;
	 
	 char *peer_host = instruction_set->host_peer_addr;
	 char *peer_port = instruction_set->host_peer_port;
	 char *peer_id   = nhpc_itostr(instruction_set->host_peer_id);
	 char *host_uid  = instruction_set->host_grid_uid;
	 
	 nhpc_socket_t *sock;
	 nrv = socket_connect(&sock, peer_host, peer_port, AF_INET, SOCK_STREAM, 0);
	 
	 if(nrv != NHPC_SUCCESS)
	 {
	    delete[] peer_id;
	    
	    return nrv;
	 }
	 
	 nhpc_headers_t *headers = new nhpc_headers_t;
	 
	 headers->insert("GRID SUBMISSION 2.90");
	 headers->insert("Grid-Uid", host_uid);
	 headers->insert("Peer", peer_id);
	 nrv = headers->write(sock);	       
	 delete headers;
	 
	 delete[] peer_id;
	 
	 socket_close(sock);
	 socket_delete(sock);
	 
	 nhpc_delete_instruction(instruction_set);
	 
	 pthread_mutex_lock(mutex_child_processes);
	 child_processes->erase(pid);
	 pthread_mutex_unlock(mutex_child_processes);
      }
      
      return NHPC_SUCCESS;
   }
   
   void grid_scheduler_t::monitor_jobs_pending(grid_scheduler_t *grid_scheduler)
   {
      rbtree_t *queued_instructions = grid_scheduler->queued_instructions;
      
      while(1)
      {
	 int id;

	 grid_scheduler->lock();
	 nhpc_instruction_set_t *instruction_set = (nhpc_instruction_set_t *)queued_instructions->search_first(&id);
	 grid_scheduler->unlock();
	 
	 if(instruction_set)
	 {
	    cout<<"Instruction Pending: "<<grid_scheduler->cores()<<endl;
	 }
	 
	 sleep(1);
      }
   }
   
   void grid_scheduler_t::child_handler(int signum)
   {
      if(signum != SIGCHLD)
	 return;
      
      scheduler->free_child_process();
   }
};
