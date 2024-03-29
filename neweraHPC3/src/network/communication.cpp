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
#include <iomanip>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/communication.h>
#include <include/error.h>
#include <include/network.h>
#include <include/http.h>

using namespace std;

namespace neweraHPC
{
   void read_communication(nhpc_socket_t *sock)
   {
      LOG_INFO("Message from: "<<sock->host<<":"<<sock->port);
      
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
      
      nhpc_display_headers(sock);
      
      char                *command = (char *)sock->headers->search("command");
      network_addon_ptr_t  fnc;
      if(command && (fnc = nhpc_network_find_addon(command)))
      {
	 fnc(sock);
      }
      else 
	 LOG_DEBUG("NETWORK COMMAND:" << command << " NOT FOUND");
      
      nhpc_socket_cleanup(sock);
   }
   
   nhpc_status_t nhpc_analyze_stream(nhpc_socket_t *sock, char *data, nhpc_size_t *len, nhpc_size_t *header_size)
   {
      if(header_size != NULL)
	 *header_size = 0;
      
      if(sock->have_headers == true)
	 return NHPC_FAIL;
      
      int line_len = 0;
      int old_pos = 0;
      
      if(sock->headers == NULL)
	 sock->headers = new rbtree(RBTREE_STR);
      rbtree *headers = sock->headers;
      
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
	       nhpc_size_t tmp_header_size = (cntr + 1);
	       if(data[cntr + 1] == '\n')
		  tmp_header_size++;
	       
	       if(header_size != NULL)
		  (*header_size) = tmp_header_size;
	       
	       if((*len) > tmp_header_size)
	       {
		  char *tmp_buffer = new char [(*len) - tmp_header_size];
		  memcpy(tmp_buffer, (data + tmp_header_size), ((*len) - tmp_header_size));

		  sock->partial_content_len = *len - tmp_header_size;
		  sock->partial_content     = tmp_buffer;
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
      rbtree *headers = sock->headers;
      
      if(headers == NULL)
	 return;
      
      const char *string;
      const char *key;

      int count = (*headers).length();
      
      LOG_INFO("Headers found in the message:" << count);
      for(int cntr = 1; cntr <= count; cntr++)
      {
	 string = (const char *)(*headers).search_inorder_str(cntr, &key);
	 
	 int width = strlen(string);
	 LOG_INFO(setw(20) << key << setw(10) << "\t" << string);
      }
   }
};
