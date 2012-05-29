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

#include <include/network.h>
#include <include/general.h>

using namespace std;

namespace neweraHPC
{
   void sig_action(int);
   
   void http_init(nhpc_socket_t *sock)
   {
      char *command = (char *)sock->headers->search("command");
      
      if(nhpc_strcmp(command, "GET*") == NHPC_SUCCESS)
	 http_request(sock);
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
	 char *file_path = nhpc_strconcat(HTTP_ROOT, request->strings[1]);
	 
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
	    delete[] file_size_str;
	    
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
      nhpc_create_tmp_file_or_dir(file_path, "neweraHPC", NHPC_FILE);
      
      const char *command = nhpc_strconcat("GET /", target_file, " HTTP/1.1");
      nhpc_headers_t *headers = new nhpc_headers_t;
      headers->insert(command);
      headers->insert("User-Agent: neweraHPC");
      headers->insert("Host", host_addr);
      headers->write(sock);
      delete headers;
      delete[] command;
      
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