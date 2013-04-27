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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/headers.h>
#include <include/sockets.h>
#include <include/strings.h>

using namespace std;

namespace neweraHPC
{
   nhpc_headers_t::nhpc_headers_t()
   {
      headers = new rbtree;
   }
   
   nhpc_headers_t::~nhpc_headers_t()
   {
      int header_count = (*headers).length();
      
      for(int i = 1; i <= header_count; i++)
      {
	 char *tmp_string = (char *)(*headers)[i];
	 nhpc_string_delete(tmp_string);
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
      char *tmp_property = nhpc_strconcat(property, ": ", argument);
      insert(tmp_property);
      delete[] tmp_property;
   }
   
   const char *nhpc_headers_t::operator[](int id)
   {
      const char *tmp_str = (const char *)headers->search(id + 1);
      return tmp_str;
   }
   
   int nhpc_headers_t::count()
   {
      return (*headers).length();
   }
   
   nhpc_status_t nhpc_headers_t::write(nhpc_socket_t *sock)
   {
      char         *str;
      nhpc_size_t   size;
      nhpc_status_t nrv;
      
      int count = (*headers).length();
      
      for(int i = 1; i <= count; i++)
      {
	 str  = (char *)headers->search(i);
	 size = strlen(str);
	 
	 nrv = socket_sendmsg(sock, str, &size);
	 
	 nhpc_string_delete(str);
	 headers->erase(i);
      }
      
      size = 2;
      nrv = socket_sendmsg(sock, "\r\n", &size);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_headers_insert_param(rbtree *headers, const char *str)
   {
      char *argument;
      
      if(headers->length() == 0)
      {
	 nhpc_strcpy(&argument, str);
	 headers->insert(argument, "command");
	 return NHPC_SUCCESS;
      }
      
      if(nhpc_strcmp(str, "*: *") == NHPC_FAIL)
	 return NHPC_FAIL;
      
      int pos  = nhpc_strfind(str, ':');
      int pos2 = nhpc_strfind(str, ' ', pos);
      if(pos2 < pos || pos2 == 0)
	 pos2 = pos;

      char *argument1 = nhpc_substr(str, 1, pos - 1);
      char *argument2 = nhpc_substr(str, pos2 + 1, strlen(str));

      headers->insert(argument2, argument1);
      delete[] argument1;
      
      return NHPC_SUCCESS;
   }
   
   void nhpc_delete_headers(rbtree *headers)
   {
      const char *key;
      char *string;
      
      int count = (*headers).length();
      
      while((string = (char *)(*headers).search_inorder_str(1, &key)))
      {
	 (*headers).erase(key);
	 delete[] string;
      }
   }
}
