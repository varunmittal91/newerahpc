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
#include <include/thread.h>
#include <include/file.h>

using namespace std;

namespace neweraHPC
{
   class thread_manager_t;
   
   void grid_init(nhpc_socket_t *sock)
   {
      cout<<"GRID Request Encountered"<<endl;
   }
   
   nhpc_grid_server_t::nhpc_grid_server_t(thread_manager_t **in_thread_manager) : plugin_manager_t(in_thread_manager)
   {
      clients = new rbtree_t(NHPC_RBTREE_STR);
   }
   
   void nhpc_grid_server_t::grid_server_init()
   {
      plugin_manager_init();
            
      mkdir("/tmp/neweraHPC", 0777);
   }
   
   nhpc_grid_server_t::~nhpc_grid_server_t()
   {
      
   }
   
   void nhpc_grid_server_t::grid_request_init(nhpc_socket_t *sock)
   {
      cout<<"GRID Request Encountered"<<endl;      
      
      header_t *header = (header_t *)sock->headers->search(1);
      string_t *string = nhpc_substr(header->string, ' ');
      
      for(int i = 0; i < string->count; i++)
	 cout<<string->strings[i]<<endl;
      
      if(string->count < 3)
      {
	 cout<<"Invalid Request"<<endl;
	 return;
      }
      
      nhpc_status_t nrv;
      const char *fnc_str = string->strings[1];
      
      if(nhpc_strcmp(fnc_str, "CLIENT_REGISTRATION") == NHPC_SUCCESS)
	 nrv = grid_client_registration(sock);
      else if(nhpc_strcmp(fnc_str, "FILE_EXCHANGE") == NHPC_SUCCESS)
	 nrv = nhpc_grid_file_download(sock, NULL);
      
      cout<<"Executed function"<<endl;
      
      nhpc_string_delete(string);
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_client_registration(nhpc_socket_t *sock)
   {
      nhpc_status_t nrv;
      nrv = NHPC_FAIL;
      
      const char *uid;
      nrv = grid_client_gen_uid(sock->host, &uid);
      
      nhpc_size_t size = strlen(uid);
      nrv = socket_send(sock, uid, &size);
      
      return nrv;      
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_client_gen_uid(const char *client_addr, const char **uid)
   {
      const char *tmp_client_addr;
      char *random_string = NULL;
      
      do 
      {
	 if(random_string != NULL)
	    delete[] random_string;
	 
	 random_string = nhpc_random_string(8);
	 
	 tmp_client_addr = (const char *)clients->search(random_string);
      }while(tmp_client_addr != NULL);

      *uid = random_string;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_grid_file_download(nhpc_socket_t *sock, const char **file_path)
   {
      int header_count = sock->headers->ret_count();
      
      if(header_count < 3)
	 return NHPC_FAIL;
      
      header_t *header = (header_t *)sock->headers->search(2);
      if(nhpc_strcmp(header->string, "File-Type: *") != NHPC_SUCCESS)
	 return NHPC_FAIL;
      
      string_t *string = nhpc_substr(header->string, ' ');
      if(string->count == 1)
      {
	 nhpc_string_delete(string);
	 return NHPC_FAIL;
      }
      
      const char *file_type;
      nhpc_strcpy((char **)&file_type, string->strings[1]);
      nhpc_string_delete(string);
      
      return NHPC_SUCCESS;
   }
};