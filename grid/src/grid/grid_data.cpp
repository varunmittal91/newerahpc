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

#include <neweraHPC/rbtree.h>
#include <neweraHPC/network.h>

#include <include/grid_data.h>

namespace neweraHPC
{
   void grid_data_create_from_socket(grid_data_t *data, nhpc_socket_t *socket)
   {
      rbtree *headers = socket->headers;
      const char *peer_addr = nhpc_socket_get_host_port(socket);
      const char *peer_port = nhpc_socket_get_host_addr(socket);
      
      nhpc_strcpy((char **)&(data->peer_addr), peer_addr);
      nhpc_strcpy((char **)&(data->peer_port), peer_port);
   }
}