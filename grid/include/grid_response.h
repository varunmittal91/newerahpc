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

#include "grid_communication.h"

namespace neweraHPC
{
#define GRID_RESPONSE_SUCCESSFUL            0
#define GRID_RESPONSE_RESOURCE_UNAVAILABLE  1
#define GRID_RESPONSE_RESOURCE_DENIED       2
   
   static struct _GRID_RESPONSE_MSSGS
   {
      const char *MSSGS_STRINGS;
   }GRID_RESPONSE_MSSGS[3] = {"RESPONSE_OK", "RESPONSE_501", "RESPONSE_403"};

   typedef unsigned char grid_response_type_t;
   struct grid_response_t
   {
      grid_response_type_t  response_type;
      nhpc_socket_t        *socket;
      nhpc_headers_t       *headers;
      void                 *grid_data;
      nhpc_size_t           grid_data_len;
   };

#define grid_get_response_status_code(gc)      ((gc->response_type) >> 3)
#define grid_set_response_status_code(gc, c)   ((gc->response_type) |= (c << 3))
#define grid_get_response_status_mssg(gc)      (GRID_RESPONSE_MSSGS[grid_get_response_status_code(gc)].MSSGS_STRINGS)
#define grid_get_response_code_status_mssg(c)  (GRID_RESPONSE_MSSGS[c].MSSGS_STRINGS)
#define grid_is_response_complete(gc)          ((gc->request_type) & 1)
#define grid_set_response_complete(gc)         ((gc->request_type) |= 1)
#define grid_set_response_type(gc, c)          ((gc->request_type) |= (c << 3))
#define grid_set_response_header(gc, h, v)     ((gc->headers->insert(h, v)))
#define grid_set_grid_data(gr, d)              ((gr->grid_data = d))
#define grid_response_set_socket(g, s)         (g->socket = s)
#define grid_response_get_grid_data(g)         (g->grid_data)
   
   static void grid_response_init(grid_response_t **grid_response)
   {
      (*grid_response) = new grid_response_t;
      memset((*grid_response), 0, sizeof(grid_response_t));
      (*grid_response)->headers = new nhpc_headers_t;
   }
   static void grid_response_destruct(grid_response_t *grid_response)
   {
      delete grid_response->headers;
      delete grid_response;
   }
   void grid_response_add_data(grid_response_t *grid_response, void *data, nhpc_size_t data_len);
   nhpc_status_t grid_response_send(grid_response_t *grid_response);
   nhpc_status_t grid_response_push(grid_response_t *grid_response);
   
   nhpc_status_t grid_response_get(grid_response_t **grid_response, grid_communication_t *grid_communication);
}