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

#include <include/network.h>
#include <include/grid.h>

using namespace std;

namespace neweraHPC
{
   void read_communication(nhpc_socket_t *sock)
   {
      sock->headers = new rbtree_t;
      
      char buffer[1000];
      nhpc_size_t size = 1000;
      nhpc_size_t content_size = 0;
      nhpc_status_t nrv = 0;
      
      while(sock->have_headers != true)
      {
	 size = 1000;
	 bzero(buffer, size);
	 
	 nrv = socket_recv(sock, buffer, &size);
	 nhpc_analyze_stream(sock, buffer, &size, &content_size);
	 
	 if(nrv == NHPC_EOF)
	    break;
      }
      
      if(content_size != 0)
      {
	 sock->partial_content_len = content_size;
	 sock->partial_content = new char [content_size];
	 memcpy(sock->partial_content, (buffer + sizeof(buffer) - content_size), content_size);
      }
      else 
	 sock->partial_content = NULL;
      
      cout<<"Message from: "<<sock->host<<":"<<sock->port<<endl;
      nhpc_display_headers(sock);
      cout<<endl;
      
      header_t *header = (header_t *)sock->headers->search(1);
      if(header != NULL)
      {
	 network_t *network = sock->server_details->main_network;
	 
	 if(nhpc_strcmp(header->string, "*HTTP*") == NHPC_SUCCESS)
	    http_init(sock);
	 else if(nhpc_strcmp(header->string, "*GRID*") == NHPC_SUCCESS)
	    network->grid_request_init(sock);
      }
      
      nhpc_socket_cleanup(sock);
   }
};
