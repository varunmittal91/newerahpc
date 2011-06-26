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
   nhpc_status_t nhpc_recv(nhpc_socket_t *sock, char *buffer, size_t *len)
   {
      int rv = 0;
      
      do {
	 rv = read(sock->sockfd, buffer, *len);
      } while (rv == -1 && errno == EINTR);
      
      while ((rv == -1) && (errno == EAGAIN || errno == EWOULDBLOCK))
      {
      do_select:
	 int nrv = nhpc_wait_for_io_or_timeout();
	 if (nrv != NHPC_SUCCESS) 
	 {
            *len = 0;
            return nrv;
	 }
	 else 
	 {
            do
	    {
	       rv = read(sock->sockfd, buffer, (*len));
            } while (rv == -1 && errno == EINTR);
	 }
      }
      
      if(rv == -1){
	 *len = 0;
	 return errno;
      }
      if((sock->timeout > 0) && (rv < *len))
      {
	 sock->options |= NHPC_POLLING_READ;
      }
      
      (*len) = rv;
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_analyze_stream(char *buffer, size_t *len)
   {
      int cntr = 0;
      char *start = buffer;
      
      cout<<buffer<<endl;
      
      while(cntr != *len)
      {
	 cout<<"("<<(int)*start<<")";
	 start++;
	 cntr++;
      }
      cout<<endl;
   }
};

