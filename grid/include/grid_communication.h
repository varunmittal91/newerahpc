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

#ifndef _GRID_COMMUNICATION_H_
#define _GRID_COMMUNICATION_H_

#include <neweraHPC/rbtree.h>
#include <neweraHPC/network.h>

#include "grid_data.h"

namespace neweraHPC
{
   static rbtree *grid_communication_handlers;
#define grid_communication_register_handler(h, m) ((*grid_communication_handlers).insert((void *)(h), grid_get_communication_code_status_mssg(m)))

#define GRID_CLIENT_REGISTRATION            0
#define GRID_NODE_REGISTRATION              1
#define GRID_FILE_EXCHANGE                  2
#define GRID_INSTRUCTION                    3
#define GRID_SUBMISSION                     4
#define GRID_PLUGIN_REQUEST                 5
   
#define GRID_COMMUNICATION_OPT_REGISTER 1
   
   static struct _GRID_STATUS_MSSGS
   {
      const char        *MSSGS_STRINGS;
   }GRID_STATUS_MSSGS[6] = {"CLIENT_REGISTRATION", "NODE_REGISTRATION", "FILE_EXCHANGE",
      "INSTRUCTION", "SUBMISSION", "PLUGIN_REQUEST"};
#define grid_get_communication_status_code(gc)      ((gc->request_type) >> 3)
#define grid_get_communication_status_mssg(gc)      (GRID_STATUS_MSSGS[grid_get_communication_status_code(gc)].MSSGS_STRINGS)
#define grid_get_communication_code_status_mssg(c)  (GRID_STATUS_MSSGS[c].MSSGS_STRINGS)
   
   typedef unsigned char grid_request_type;      
   struct grid_communication_t
   {
      grid_request_type  request_type;
      nhpc_headers_t    *headers;
      nhpc_socket_t     *socket;
      const char        *dest_addr;
      const char        *dest_port;
      const char        *peer_addr;
      const char        *peer_port;
   };
#define grid_is_communication_complete(gc)    ((gc->request_type) & 1)
#define grid_set_communication_complete(gc)   ((gc->request_type) |= 1)
#define grid_set_communication_type(gc, c)    ((gc->request_type) |= (c << 3))
#define grid_set_communication_header(gc,h,v) ((gc->headers->insert(h, v)))

#define grid_set_communication_opt(gc, o)      ((gc->request_type) |= (o << 1)) 
#define grid_communication_opt_is_register(gc) (1 & (gc->request_type) >> 1)
   
   static void grid_communication_init(grid_communication_t **gc, grid_request_type c)
   {
      (*gc) = new grid_communication_t;
      memset((*gc), 0, sizeof(grid_communication_t));
      grid_set_communication_type((*gc), c);
   }
   static void grid_communication_destruct(grid_communication_t *grid_communication)
   {
      if(grid_communication->dest_addr)
	 delete[] grid_communication->dest_addr;
      if(grid_communication->dest_port)
	 delete[] grid_communication->dest_port;
      delete grid_communication->headers;
      
      delete grid_communication;
   }   
   
   static void grid_communication_add_dest(grid_communication_t *grid_commuincation, const char *dest_addr, const char *dest_port)
   {
      nhpc_strcpy((char **)&(grid_commuincation->dest_addr), dest_addr);
      nhpc_strcpy((char **)&(grid_commuincation->dest_port), dest_port);
   }
#define grid_communication_add_peer(gc, g)  (gc->peer_addr = g->host_addr, gc->peer_port = g->host_port);
   
   nhpc_status_t grid_communication_send(grid_communication_t *grid_communication);
   nhpc_status_t grid_communication_push(grid_communication_t *grid_communication);
   
   nhpc_status_t grid_communication_handlers_init();
   nhpc_status_t grid_request_handler(nhpc_socket_t *socket);
}

#endif