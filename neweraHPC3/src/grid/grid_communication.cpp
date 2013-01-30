/*
 *
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/grid.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC 
{
   nhpc_status_t nhpc_grid_communication_send(grid_communication_t *grid_communication)
   {
      nhpc_status_t nrv;
      const char *grid_uid = NULL;
      
      const char *mssg = nhpc_grid_get_communication_status_mssg(grid_communication);
      
      if(nhpc_grid_communication_opt_is_register(grid_communication))
      {
	 nrv = nhpc_register_to_server(&grid_uid, grid_communication->dest_addr, grid_communication->dest_port);
	 if(nrv == NHPC_FAIL)
	 {
	    return nrv;
	 }
      }
      
      const char *header_str = nhpc_strconcat("GRID ", mssg, " 2.90");
      grid_communication->headers = new nhpc_headers_t;
      grid_communication->headers->insert(header_str);
      if(grid_uid)
      {
	 grid_communication->headers->insert("Grid-Uid", grid_uid);
      }
      grid_communication->headers->insert("Peer-Host", grid_communication->peer_addr);
      grid_communication->headers->insert("Peer-Port", grid_communication->peer_port);      
      
      delete[] header_str;
   }
   
   nhpc_status_t nhpc_grid_communication_push(grid_communication_t *grid_communication)
   {
      const char *dest_addr = grid_communication->dest_addr;
      const char *dest_port = grid_communication->dest_port;
      nhpc_status_t   nrv;
      nhpc_socket_t  *socket;
      nhpc_headers_t *headers = grid_communication->headers;
      
      nrv = socket_connect(&socket, dest_addr, dest_port, AF_INET, SOCK_STREAM, 0);
      nrv = headers->write(socket);	          
      
      socket_close(socket);
      socket_delete(socket);
      
      return nrv;
   }
}