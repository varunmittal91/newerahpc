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

#include <iostream>
#include <fstream>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/network.h>
#include <include/general.h>
#include <include/web_ui.h>

using namespace std;

namespace neweraHPC
{
   rbtree_t *http_handlers;
   void sig_action(int);
   
   nhpc_status_t http_handler_register(const char *handler_string, fnc_ptr_nhpc_t handler_function)
   {
      fnc_ptr_nhpc_t *func_trigger_local = new fnc_ptr_nhpc_t;
      (*func_trigger_local) = handler_function;
      
      nhpc_status_t rv = http_handlers->insert(func_trigger_local, handler_string);
      
      return rv;      
   }
   
   void http_init()
   {
      LOG_INFO("Initialize http handler");
      http_handlers = new rbtree_t(NHPC_RBTREE_STR);
   }
   
   void http_init(nhpc_socket_t *sock)
   {
      char *command = (char *)sock->headers->search("command");
      
      if((nhpc_strcmp(command, "GET*") == NHPC_SUCCESS) || (nhpc_strcmp(command, "POST*") == NHPC_SUCCESS))
      {
	 if((nhpc_strcmp(command, "POST*") == NHPC_SUCCESS) == NHPC_SUCCESS)
	    cout<<sock->partial_content<<endl;
	 
	 string_t *tmp_str = nhpc_substr(command, ' ');
	 string_t *tmp_str2 = NULL;
	 char *app_name = NULL;
	 
	 if(tmp_str->count == 3)
	 {
	    tmp_str2 = nhpc_substr(tmp_str->strings[1], '/');
	    app_name = tmp_str2->strings[0];
	 }
	 else 
	 {
	    nhpc_string_delete(tmp_str);
	    return;
	 }

	 LOG_INFO("Checking for: " << app_name);
	 fnc_ptr_nhpc_t *func_trigger_local = (fnc_ptr_nhpc_t *)http_handlers->search(app_name);
	 if(func_trigger_local != NULL)
	 {
	    LOG_INFO("Found http handler: " << app_name);
	    nhpc_status_t nrv = (*func_trigger_local)(sock);
	 }
	 
	 nhpc_string_delete(tmp_str);
	 
	 http_request(sock);
      }
      else if(nhpc_strcmp(command, "HTTP*") == NHPC_SUCCESS)
	 cout<<"HTTP Response"<<endl;
      else 
	 cout<<"Invalid HTTP Header"<<endl;
   }
   
   void http_request(nhpc_socket_t *sock)
   {
      char *command = (char *)sock->headers->search("command");
      string_t *request = nhpc_substr(command, ' ');
      
      if(request->count < 3)
      {
	 const char *mssg = "HTTP/1.1 403 Invalid Request\r\n\r\nInvalid request\r\n";
	 nhpc_size_t size = strlen(mssg);
	 socket_send(sock, (char *)mssg, &size);
      }
      else 
      {
	 char *file_path = NULL;
	 
	 if(nhpc_strcmp((request->strings[1]), "/app/*") == NHPC_SUCCESS)
	 {
	    rbtree_t *ui_details;
	    
	    nhpc_status_t nrv = web_ui_init_request(sock, request, &ui_details, &file_path);
	 }
	 else 
	    file_path = nhpc_strconcat(HTTP_ROOT, request->strings[1]);
	 
	 nhpc_size_t file_size;
	 nhpc_status_t nrv = nhpc_file_size(file_path, &file_size);
	 
	 if(nrv == NHPC_FILE_NOT_FOUND)
	 {
	    const char *mssg = "HTTP/1.1 404 Content Not Found\r\n\r\nContent Not Found\r\n";
	    nhpc_size_t size = strlen(mssg);
	    socket_send(sock, (char *)mssg, &size);
	 }
	 else if(nrv == NHPC_DIRECTORY)
	 {
	    const char *mssg = "HTTP/1.1 404 Content Not Found\r\n\r\nServer Doesn't Know How To Handle Directory\r\n";
	    nhpc_size_t size = strlen(mssg);
	    socket_send(sock, (char *)mssg, &size);
	 }
	 else 
	 {
	    FILE *fp = fopen(file_path, "r");
	    char *file_size_str = nhpc_itostr(file_size);

	    nhpc_headers_t *headers = new nhpc_headers_t;
	    headers->insert("HTTP/1.1 200 OK");
	    headers->insert("Content-Length", file_size_str);
	    headers->write(sock);
	    
	    delete headers;
	    nhpc_string_delete(file_size_str);
	    
	    nhpc_status_t nrv;
	    
	    char buffer[10000];	    
	    nhpc_size_t len;
	    
	    do
	    {
	       bzero(buffer, sizeof(buffer));
	       len = fread(buffer, 1, sizeof(buffer), fp);
	       
	       nrv = socket_sendmsg(sock, buffer, &len);	
	    }while(!feof(fp) && errno != EPIPE);
		   
	    fclose(fp);
	 }
	 
	 nhpc_string_delete(file_path);
      }
      
      nhpc_string_delete(request);
   }
   
   nhpc_status_t http_get_file(const char **file_path, nhpc_socket_t *sock, const char *target_file, const char *host_addr)
   {
      nhpc_create_tmp_file_or_dir(file_path, "/tmp/neweraHPC", NHPC_FILE);
      
      const char *command = nhpc_strconcat("GET /", target_file, " HTTP/1.1");
      nhpc_headers_t *headers = new nhpc_headers_t;
      headers->insert(command);
      headers->insert("User-Agent: neweraHPC");
      headers->insert("Host", host_addr);
      headers->write(sock);
      delete headers;
      nhpc_string_delete((char *)command);
      
      FILE *fp = fopen(*file_path, "w+");
      nhpc_status_t nrv;
      nhpc_size_t size;
      nhpc_size_t size_downloaded = 0;
      nhpc_size_t file_size;
      
      char buffer[10000];
      nhpc_size_t header_size = 0;
      
      do 
      {
	 bzero(buffer, sizeof(buffer));
	 size = sizeof(buffer);
	 header_size = 0;
	 
	 do 
	 {
	    nrv = socket_recv(sock, buffer, &size);
	 }while(nrv != NHPC_SUCCESS && nrv != NHPC_EOF);
	 
	 if(sock->have_headers == false)
	 {
	    nrv = nhpc_analyze_stream(sock, buffer, &size, &header_size);
	    if(nrv == NHPC_SUCCESS)
	    {
	       http_content_length(sock->headers, &file_size);
	       nhpc_display_headers(sock);
	    }
	 }
	 
	 fwrite((buffer + header_size), 1, (size - header_size), fp); 
	 
	 size_downloaded += (size - header_size);
      }while(nrv != NHPC_EOF && size_downloaded != file_size);
      
      cout<<"Size Downloaded: "<<size_downloaded<<endl;
      
      fclose(fp);
      
      return nrv;
   }
   
   void http_response(nhpc_socket_t *sock)
   {
      
   }
};