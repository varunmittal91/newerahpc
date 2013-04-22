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

#include <include/grid_response.h>

namespace neweraHPC
{
   nhpc_status_t grid_response_get(grid_response_t **grid_response, grid_communication_t *grid_communication)
   {
      (*grid_response) = new grid_response_t;
      memset((*grid_response), 0, sizeof(grid_response_t));
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
      
      const char *header_string = nhpc_strconcat("GRID/1.1 ", mssg);
      grid_response->headers    = new nhpc_headers_t;
      grid_response->headers->insert(header_string);      
      if(grid_response->grid_data)
      {
	 const char *grid_data_len_str = nhpc_itostr(grid_response->grid_data_len);
	 grid_response->headers->insert("Content-Length", grid_data_len_str);

	 delete[] grid_data_len_str;
      }
      delete[] header_string;
      
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