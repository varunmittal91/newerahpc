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
   nhpc_status_t nhpc_recv(int sockfd, char *buffer, size_t *len)
   {
      int rv = 0;
      
      do {
	 rv = read(sockfd, buffer, *len);
      } while (rv == -1 && errno == EINTR);
      
      while ((rv == -1) && (errno == EAGAIN || errno == EWOULDBLOCK))
      {
	 int arv = nhpc_wait_for_io_or_timeout();
	 if (arv != NHPC_SUCCESS) 
	 {
            *len = 0;
            return arv;
	 }
	 else 
	 {
            do
	    {
	       rv = read(sockfd, buffer, (*len));
            } while (rv == -1 && errno == EINTR);
	 }
      }
   }
};

