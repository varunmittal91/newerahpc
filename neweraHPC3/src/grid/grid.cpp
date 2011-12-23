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
      
      if(string->count < 3)
      {
	 cout<<"Invalid Request"<<endl;
	 return;
      }
      
      nhpc_status_t nrv;
      const char *fnc_str = string->strings[1];
      
      if(nhpc_strcmp(fnc_str, "CLIENT_REGISTRATION") == NHPC_SUCCESS)
	 nrv = grid_client_registration(sock);
      else 
      {
	 header_t *header = (header_t *)sock->headers->search(2);
	 const char *uid;
	 
	 if(sock->headers->ret_count() < 2 || grid_client_verify_uid(header, &uid) != NHPC_SUCCESS)
	 {
	    cout<<"GRID UID Not Supplied"<<endl;
	    nhpc_string_delete(string);
	    return;
	 }
	 
	 else if(nhpc_strcmp(fnc_str, "FILE_EXCHANGE") == NHPC_SUCCESS)
	 {
	    nrv = grid_file_download(sock, &uid);
	 }
	 
	 char *response = nhpc_itostr(nrv);
	 nhpc_size_t size = strlen(response);
	 socket_send(sock, response, &size);
      }
      
      cout<<"Executed function with status: "<<nrv<<endl;
      
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
      
      const char *dir = nhpc_strconcat(grid_directory, uid);
      mkdir(dir, 0777);
      delete[] dir;
      
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
      clients->insert((void *)client_addr, random_string);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_client_verify_uid(header_t *header, const char **uid)
   {
      if(nhpc_strcmp(header->string, "Grid-Uid: *") != NHPC_SUCCESS)
      {	 
	 return NHPC_FAIL;
      }
      
      string_t *string = nhpc_substr(header->string, ' ');
      if(string->count == 1)
      {
	 nhpc_string_delete(string);
	 return NHPC_FAIL;
      }
      
      if(clients->search(string->strings[1]) == NULL)
      {
	 nhpc_string_delete(string);
	 return NHPC_FAIL;
      }
      
      nhpc_strcpy((char **)uid, string->strings[1]);
      nhpc_string_delete(string);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_file_download(nhpc_socket_t *sock, const char **grid_uid)
   {
      int header_count = sock->headers->ret_count();
      
      if(header_count < 4)
	 return NHPC_FAIL;
      
      header_t *header = (header_t *)sock->headers->search(3);
      if(nhpc_strcmp(header->string, "File-Type: *") != NHPC_SUCCESS)
      {	 
	 return NHPC_FAIL;
      }
      
      string_t *string = nhpc_substr(header->string, ' ');
      if(string->count == 1)
      {
	 nhpc_string_delete(string);
	 return NHPC_FAIL;
      }
      
      const char *file_type;
      nhpc_strcpy((char **)&file_type, string->strings[1]);
      nhpc_string_delete(string);
      
      header = (header_t *)sock->headers->search(4);
      if(nhpc_strcmp(header->string, "File-Name: *") != NHPC_SUCCESS)
	 return NHPC_FAIL;
      
      string = nhpc_substr(header->string, ' ');
      if(string->count == 1)
      {
	 nhpc_string_delete(string);
	 return NHPC_FAIL;
      }
      
      const char *file_name;
      nhpc_strcpy((char **)&file_name, string->strings[1]);
      nhpc_string_delete(string);
      
      header = (header_t *)sock->headers->search(5);
      if(nhpc_strcmp(header->string, "Content-Length: *") != NHPC_SUCCESS)
	 return NHPC_FAIL;
      
      string = nhpc_substr(header->string, ' ');
      if(string->count == 1)
      {
	 nhpc_string_delete(string);
	 return NHPC_FAIL;
      }
      
      nhpc_size_t file_size = nhpc_strtoi((const char *)(string->strings[1]));
      nhpc_string_delete(string);      
      if(file_size == 0)
	 return NHPC_FAIL;
      
      cout<<"File Name: "<<file_name<<endl;
      cout<<"File Type: "<<file_type<<endl;
      cout<<"File Size: "<<file_size<<endl;
      
      nhpc_size_t size_downloaded = 0;
      char buffer[1000];
      nhpc_status_t nrv;
      nhpc_size_t size;
      
      const char *grid_path = nhpc_strconcat("/www/grid/", *grid_uid);
      const char *tmp_path = nhpc_strconcat(grid_path, "/");
      const char *final_path = nhpc_strconcat(tmp_path, file_name);

      FILE *fp = fopen(final_path, "w+");

      delete[] grid_path;
      delete[] tmp_path;
      
      do 
      {
	 bzero(buffer, 1000);
	 size = 1000;
	 nrv = socket_recv(sock, buffer, &size); 
	 fwrite(buffer, 1, size, fp);	 
	 size_downloaded += size;
	 cout<<buffer<<endl;
	 cout<<file_size<<" "<<size_downloaded<<" "<<nrv<<" "<<size<<endl;
      }while((nrv != NHPC_EOF) && file_size != size_downloaded);
      
      fclose(fp);
      
      if(nhpc_strcmp(file_type, "plugin") == NHPC_SUCCESS)
      {
	 nrv = install_plugin(final_path, *grid_uid);
	 if(nrv != NHPC_SUCCESS)
	    return NHPC_FAIL;
      }
      
      delete[] file_name;
      delete[] file_type;
      delete[] final_path;
      
      return NHPC_SUCCESS;
   }
};