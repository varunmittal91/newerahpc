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
      cout<<"Message from: "<<sock->host<<":"<<sock->port<<endl;
      
      sock->headers = new rbtree_t(NHPC_RBTREE_STR);
      sock->partial_content = NULL;
      sock->partial_content_len = 0;
      
      char buffer[1000];
      nhpc_size_t size = 1000;
      nhpc_size_t header_size = 0;
      nhpc_status_t nrv = 0;
      
      while(sock->have_headers != true)
      {
	 size = 1000;
	 bzero(buffer, size);
	 
	 nrv = socket_recv(sock, buffer, &size);
	 nhpc_analyze_stream(sock, buffer, &size, &header_size);
	 
	 if(nrv == NHPC_EOF)
	    break;
      }
      
      if(header_size != 0)
      {
	 sock->partial_content_len = size - header_size;
	 sock->partial_content = new char [size - header_size];
	 memcpy(sock->partial_content, (buffer + header_size), (size - header_size));
      }
      else 
	 sock->partial_content = NULL;
      
      nhpc_display_headers(sock);
      cout<<endl;
            
      char *command = (char *)sock->headers->search("command");
      if(command != NULL)
      {
	 network_t *network = sock->server_details->main_network;
	 
	 if(nhpc_strcmp(command, "*HTTP*") == NHPC_SUCCESS)
	    http_init(sock);
	 else if(nhpc_strcmp(command, "*GRID*") == NHPC_SUCCESS)
	    network->grid_request_init(sock);
      }
      
      if(sock->partial_content != NULL)
	 delete[] sock->partial_content;
      
      nhpc_socket_cleanup(sock);
   }
   
   nhpc_status_t nhpc_analyze_stream(nhpc_socket_t *sock, char *data, nhpc_size_t *len, nhpc_size_t *header_size)
   {
      if(!(sock->headers))
	 sock->headers = new rbtree_t(NHPC_RBTREE_STR);
      
      if(header_size != NULL)
	 *header_size = 0;
      
      if(sock->have_headers == true)
	 return NHPC_FAIL;
      
      int line_len = 0;
      int old_pos = 0;
      rbtree_t *headers = sock->headers;
      
      for(int cntr = 0; cntr < *len; cntr++)
      {
	 if(data[cntr] == '\r')
	 {
	    line_len = cntr - old_pos;
	    if(line_len != 0)
	    {
	       char *line = new char [line_len + 1];
	       memcpy(line, (data + old_pos), (line_len));
	       line[line_len] = '\0';
	       
	       nhpc_headers_insert_param(headers, (const char *)line);
	       delete[] line;
	    }
	    else 
	    {
	       if(header_size != NULL)
	       {
		  (*header_size) = (cntr + 1);
		  if(data[cntr + 1] == '\n')
		     (*header_size)++;
	       }
	       
	       sock->have_headers = true;
	       return NHPC_SUCCESS;
	    }
	    
	    line_len = 0;
	 }
	 else if(data[cntr] == '\n')
	 {
	    old_pos = cntr + 1;
	 }
      }
      return NHPC_FAIL;
   }
   
   void nhpc_display_headers(nhpc_socket_t *sock)
   {
      rbtree_t *headers = sock->headers;
      
      if(headers == NULL)
	 return;
      
      const char *string;
      key_pair_t *key_pair;
      
      cout<<"Headers found in the message:"<<(*headers).ret_count()<<endl;
      for(int cntr = 1; cntr <= (*headers).ret_count(); cntr++)
      {
	 key_pair = (key_pair_t *)(*headers).search_str(cntr);
	 if(key_pair != NULL)
	 {
	    cout<<" "<<key_pair->key<<"\t"<<(char *)(key_pair->data)<<endl;
	    delete key_pair;
	 }
      }
   }
};
