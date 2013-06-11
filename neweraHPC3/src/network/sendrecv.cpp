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
#include <errno.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t socket_recv(nhpc_socket_t *sock, char *buffer, nhpc_size_t *length)
   {
      int rv;
      nhpc_status_t nrv;
      
      bzero(buffer, *length);
      
      do 
      {	    
	 nrv = nhpc_wait_for_io_or_timeout(sock, 1);
	 if(nrv != NHPC_SUCCESS)
	 {
	    *length = 0;
	    return nrv;
	 }
	 rv = read((sock->sockfd), buffer, *length);
      }while(rv == -1 && errno == EINTR);
	
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
      nhpc_status_t nrv;
      
      do
      {
	 nrv = nhpc_wait_for_io_or_timeout(sock, 0);
	 if(nrv != NHPC_SUCCESS)
	 {
	    *length = 0;
	    return nrv;
	 }
	 rv = write(sock->sockfd, buffer, *length);
      }while(rv == -1 && errno == EINTR);
	     
      if(rv == -1)
      {
	 *length = 0;
	 return errno;
      }
      
      *length = rv;
      
      return NHPC_SUCCESS;
   }   
   
   nhpc_status_t socket_sendmsg(nhpc_socket_t *sock, const char *buffer, nhpc_size_t *length)
   {
      nhpc_size_t data_sent = 0;
      nhpc_size_t size = 0;
      nhpc_status_t nrv;
      
      do 
      {
	 size = *length - data_sent;
	 nrv = socket_send(sock, (buffer + data_sent), &size);
	 data_sent += size;
      }while(data_sent != *length && nrv != EPIPE);
      
      return nrv;
   }
   
   nhpc_status_t socket_sendfile(nhpc_socket_t *socket, const char *file_name)
   {
      FILE *fp = fopen(file_name, "r");
      nhpc_size_t   len;
      nhpc_status_t nrv;
      
      char        buffer[10000];
      do 
      {
	 len = fread(buffer, 1, sizeof(buffer), fp);
	 nrv = socket_sendmsg(socket, buffer, &len);
      }while(nrv != EPIPE && !feof(fp) && nrv != NHPC_FAIL);
      
      fclose(fp);
      
      return nrv;      
   }
};
