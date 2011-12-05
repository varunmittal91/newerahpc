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
   void http_init(nhpc_socket_t *sock, char *partial_content)
   {
      header_t *header = (header_t *)sock->headers->search(1);
      
      if(nhpc_strcmp(header->string, "GET*") == NHPC_SUCCESS)
	 cout<<"HTTP Request"<<endl;
      else if(nhpc_strcmp(header->string, "HTTP*") == NHPC_SUCCESS)
	 cout<<"HTTP Response"<<endl;
      else 
	 cout<<"Invalid HTTP Header"<<endl;
   }

};