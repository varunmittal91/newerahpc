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

#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   void read_communication(nhpc_socket_t *sock, char *remaining_data)
   {
      if(sock->headers != NULL)
      {
	 header_t *header = (header_t *)sock->headers->search(1);
	 cout<<sock->headers->ret_count()<<endl;
	 cout<<header->string<<endl;
	 cout<<strcmp(header->string, "GET")<<endl;
	 if(header != NULL)
	 {
	    if(nhpc_strcmp(header->string, "GET*HTTP/1*") == NHPC_SUCCESS)
	       cout<<"HTTP 1.1 Communication Detected"<<endl;
	 }
      }
   }
};