/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
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

#include <include/grid_response.h>
#include <include/grid_data_download.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t grid_response_get(grid_response_t **grid_response, grid_communication_t *grid_communication)
   {
      (*grid_response) = new grid_response_t;
      memset((*grid_response), 0, sizeof(grid_response_t));
      
      nhpc_socket_t *socket = grid_communication->socket;
      char           buffer[1000];
      nhpc_size_t    header_size = 0;
      nhpc_size_t    size;
      nhpc_status_t  nrv;
      int            response_code;

      const char    *command_str = NULL;
      const char    *check_content_len;
      string_t      *command_parts = NULL;
      
      while((socket->have_headers) != true)
      {
	 size = 1000;
	 bzero(buffer, size);
	 
	 nrv = socket_recv(socket, buffer, &size);
	 nhpc_analyze_stream(socket, buffer, &size, &header_size);
	 
	 if(nrv == NHPC_EOF)
	    break;
      }                  
      nhpc_display_headers(socket);
      
      command_str = (const char *)socket->headers->search("command");
      command_parts = nhpc_substr(command_str, ' ');
      if(command_parts->count < 3)
      {
	 nrv = NHPC_FAIL;
	 goto return_response;
      }
      response_code = nhpc_strtoi(command_parts->strings[2]);
      grid_set_response_status_code((*grid_response), response_code);
      
      check_content_len = (const char *)socket->headers->search("Content-Length");
      if(check_content_len)
      {
	 nhpc_size_t content_len = nhpc_strtoi(check_content_len);
	 
	 void *data;
	 nrv = grid_data_download_memory_block(&data, socket, &content_len);

	 grid_response_add_data((*grid_response), data, content_len);
      }
      
   return_response:
      if(command_parts)
	 nhpc_string_delete(command_parts);
      
      return nrv;
   }   
   
   void grid_response_add_data(grid_response_t *grid_response, void *data, nhpc_size_t data_len)
   {
      grid_response->grid_data     = data;
      grid_response->grid_data_len = data_len;
   }
   
   nhpc_status_t grid_response_send(grid_response_t *grid_response)
   {
      const char *grid_uid = NULL;
      const char *mssg     = grid_get_response_status_mssg(grid_response);

      int         response_code = grid_get_response_status_code(grid_response);
      const char *response_str  = nhpc_itostr(response_code); 
      
      const char *header_string = nhpc_strconcat("GRID/1.1 ", mssg, " ", response_str);
      grid_response->headers    = new nhpc_headers_t;
      grid_response->headers->insert(header_string);
      if(grid_response->grid_data)
      {
	 const char *grid_data_len_str = nhpc_itostr(grid_response->grid_data_len);
	 grid_response->headers->insert("Content-Length", grid_data_len_str);

	 delete[] grid_data_len_str;
      }
      delete[] header_string;
      delete[] response_str;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t grid_response_push(grid_response_t *grid_response)
   {
      nhpc_status_t nrv;
      
      nhpc_socket_t  *socket  = grid_response->socket;
      nhpc_headers_t *headers = grid_response->headers;
      nhpc_size_t size        = grid_response->grid_data_len;
      
      nrv = headers->write(socket);
      if(grid_response->grid_data && nrv == NHPC_SUCCESS)
	 nrv = socket_sendmsg(socket, (const char *)(grid_response->grid_data), &size);
      
      return nrv;
   }
}