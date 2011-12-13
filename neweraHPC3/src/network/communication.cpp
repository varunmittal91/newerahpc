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

using namespace std;

namespace neweraHPC
{
   void read_communication(nhpc_socket_t *sock)
   {
      sock->headers = new rbtree_t;
      
      char buffer[1000];
      nhpc_size_t size = 1000;
      nhpc_status_t nrv = 0;
      
      while(sock->have_headers != true)
      {
	 size = 1000;
	 bzero(buffer, size);
	 
	 nrv = socket_recv(sock, buffer, &size);
	 nhpc_analyze_stream(sock, buffer, &size, NULL);
	 
	 if(nrv == NHPC_EOF)
	    break;
      }
      
      header_t *header = (header_t *)sock->headers->search(1);
      if(header != NULL)
      {
	 if(nhpc_strcmp(header->string, "*HTTP*") == NHPC_SUCCESS)
	    http_init(sock);
      }
      
      nhpc_socket_cleanup(sock);
   }
};
