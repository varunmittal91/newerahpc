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
      header_t *header = (header_t *)sock->headers->search(1);
      
      if(nhpc_strcmp(header->string, "GET*") == NHPC_SUCCESS)
	 http_request(sock);
      else if(nhpc_strcmp(header->string, "HTTP*") == NHPC_SUCCESS)
	 cout<<"HTTP Response"<<endl;
      else 
	 cout<<"Invalid HTTP Header"<<endl;
   }
   
   void http_request(nhpc_socket_t *sock)
   {
      signal(SIGPIPE, SIG_IGN);
      
      header_t *header = (header_t *)sock->headers->search(1);
      string_t *request = nhpc_substr(header->string, ' ');
      
      if(request->count < 3)
      {
	 const char *mssg = "HTTP/1.1 403 Invalid Request\r\n\r\nInvalid request\r\n";
	 nhpc_size_t size = strlen(mssg);
	 socket_send(sock, (char *)mssg, &size);
      }
      else 
      {
	 nhpc_display_headers(sock);
	 
	 char *file_url = nhpc_strconcat(HTTP_ROOT, request->strings[1]);
	 
	 FILE *fp = fopen(file_url, "r");
	 if(fp == NULL)
	 {
	    const char *mssg = "HTTP/1.1 404 Content Not Found\r\n\r\nContent Not Found\r\n";
	    nhpc_size_t size = strlen(mssg);
	    socket_send(sock, (char *)mssg, &size);
	 }
	 else 
	 {
	    struct stat bufferx;
	    stat(file_url, &bufferx);
	    
	    size_t file_size = bufferx.st_size;
	    
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
   
   void http_response(nhpc_socket_t *sock)
   {
      
   }
};