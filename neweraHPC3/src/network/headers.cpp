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

#include <include/headers.h>
#include <include/sockets.h>

using namespace std;

namespace neweraHPC
{
   nhpc_headers_t::nhpc_headers_t()
   {
      headers = new rbtree_t;
   }
   
   nhpc_headers_t::~nhpc_headers_t()
   {
      for(int i = 1; i <= headers->ret_count(); i++)
      {
	 const char *tmp_string = (const char *)headers->search(i);
	 delete[] tmp_string;
	 headers->erase(i);
      }
      
      delete headers;
   }
   
   void nhpc_headers_t::insert(const char *string)
   {
      char *new_str = nhpc_strconcat(string, "\r\n");
      
      headers->insert(new_str);
   }
   
   void nhpc_headers_t::insert(const char *property, const char *argument)
   {
      char *tmp_property = nhpc_strconcat(property, ": ");
      char *new_str = nhpc_strconcat(tmp_property, argument);
      delete[] tmp_property;
      insert(new_str);
   }
   
   const char *nhpc_headers_t::operator[](int id)
   {
      const char *tmp_str = (const char *)headers->search(id + 1);
      
      return tmp_str;
   }
   
   int nhpc_headers_t::count()
   {
      return headers->ret_count();
   }
   
   nhpc_status_t nhpc_headers_t::write(nhpc_socket_t *sock)
   {
      nhpc_size_t size;
      nhpc_status_t nrv;
      
      int count = headers->ret_count();
      
      for(int i = 1; i <= count; i++)
      {
	 char *str = (char *)headers->search(i);
	 size = strlen(str);
	 
	 nrv = socket_send(sock, str, &size);
	 if(nrv != NHPC_SUCCESS)
	    return NHPC_FAIL;
	 
	 delete[] str;
	 headers->erase(i);
      }
      
      size = 2;
      nrv = socket_send(sock, "\r\n", &size);
      
      return NHPC_SUCCESS;
   }
}