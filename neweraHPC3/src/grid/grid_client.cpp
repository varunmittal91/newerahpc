/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
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

#include <include/grid.h>
#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t nhpc_register_to_server(const char **grid_uid, const char *host_addr, const char *host_port)
   {
      nhpc_socket_t *sock;
      
      nhpc_status_t nrv = socket_connect(&sock, host_addr, host_port, AF_INET, SOCK_STREAM, 0);
      
      if(nrv != NHPC_SUCCESS)
      {
	 socket_delete(sock);
	 
	 return NHPC_FAIL;
      }
      
      const char *mssg = "GRID CLIENT_REGISTRATION 2.90\r\n\r\n";
      size_t size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
      {
	 socket_close(sock);
	 socket_delete(sock);
	 
	 return NHPC_FAIL;
      }
      
      size = 1000;
      char buffer[1000];
      
      nrv = 0;
      
      nrv = socket_recv(sock, buffer, &size);
      if(nrv == NHPC_SUCCESS)
      {
	 nhpc_strcpy((char **)grid_uid, buffer);
      }
      else 
	 nrv = NHPC_FAIL;
      
      socket_close(sock);
      socket_delete(sock);
      
      return nrv;
   }
   
   nhpc_status_t nhpc_send_plugin(const char *plugin_path)
   {
      const char *mssg = "GRID FILE_UPLOAD 2.90\r\n\r\n";
      size_t size = strlen(mssg);
   }
};