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

	    const char *mssg = "HTTP/1.1 200 OK\r\nContent-Length: ";
	    char *file_size_str = nhpc_itostr(file_size);
	    mssg = nhpc_strconcat(mssg, file_size_str);
	    mssg = nhpc_strconcat(mssg, "\r\n\r\n");
	    nhpc_size_t size = strlen(mssg);
	    socket_send(sock, (char *)mssg, &size);

	    nhpc_status_t nrv;
	    
	    char buffer[10000];	    
	    int len;
	    
	    do
	    {
	       len = fread(buffer, 1, sizeof(buffer), fp);
	       size = len;
	       
	       nrv = socket_send(sock, buffer, &size);	
	    }while(nrv != EPIPE && len != 0);
		   
	    fclose(fp);
	 }
      }
   }
   
   nhpc_status_t http_get_file(const char **file_path, nhpc_socket_t *sock, const char *target_file, const char *host_addr)
   {
      nhpc_status_t nrv;
      
      nhpc_create_tmp_file_or_dir(file_path, "neweraHPC", NHPC_FILE);
      
      const char *mssg = "GET /";
      size_t size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
	 return nrv;
      
      mssg = target_file;
      size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
	 return nrv;
      
      mssg = " HTTP/1.1\r\nUser-Agent: newera\r\n";
      size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
	 return nrv;
      
      mssg = "Host: ";
      size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
	 return nrv;

      mssg = host_addr;
      size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
	 return nrv;

      mssg = "\r\n\r\n";
      size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
	 return nrv;

      FILE *fp = fopen(*file_path, "w+");
      char *buffer = new char [1000];
      size = 1000;
      int rv = 0;
      
      nhpc_size_t total_size = 0;
      nhpc_size_t file_size = -1;
      
      while((rv != NHPC_EOF && size != 0) && file_size != total_size)
      {
	 size = 1000;
	 
	 bzero(buffer, 1000);
	 rv = socket_recv(sock, buffer, &size);
	 
	 nhpc_size_t data_size;
	 
	 nrv = nhpc_analyze_stream(sock, buffer, &size, &data_size);
	 if(nrv == NHPC_SUCCESS)
	    http_content_length(sock->headers, &file_size);
	 
	 char *tmp_buffer = buffer + (size - data_size);
	 fwrite(tmp_buffer, 1, data_size, fp);
	 total_size += data_size;
	 
	 if(size == 0)
	    continue;
      }

      fclose(fp);
      
      return nrv;
   }
   
   void http_response(nhpc_socket_t *sock)
   {
      
   }
};