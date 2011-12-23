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

#include <include/network.h>
#include <iostream>
#include <errno.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t socket_recv(nhpc_socket_t *sock, char *buffer, nhpc_size_t *length)
   {
      int rv;
      nhpc_status_t nrv;
      
      bzero(buffer, *length);
      
      nrv = nhpc_wait_for_io_or_timeout(sock, 1);
      if(nrv != NHPC_SUCCESS)
      {
	 *length = 0;
	 return errno;
      }
      rv = read(sock->sockfd, buffer, *length);
	
      if(rv == -1)
      {
	 *length = 0;
	 return errno;
      }
      
      if(rv == 0)
      {
	 *length = 0;
	 return NHPC_EOF;
      }
      
      *length = rv;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t socket_send(nhpc_socket_t *sock, const char *buffer, nhpc_size_t *length)
   {
      int rv;
      int nrv;
      
      nhpc_size_t data_sent = 0;
      
      do 
      {
	 nrv = nhpc_wait_for_io_or_timeout(sock, 0);
	 rv = write(sock->sockfd, (buffer + data_sent), (*length - data_sent));
	 
	 data_sent += rv;
      }while(data_sent != *length && errno != EPIPE);
	     
      if(rv == -1)
      {
	 *length = 0;
	 return errno;
      }
      
      return NHPC_SUCCESS;
   }   
   
   nhpc_status_t socket_send_file(nhpc_socket_t *sock)
   {
      
   }
};