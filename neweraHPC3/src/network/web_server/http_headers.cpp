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

#include <include/http.h>
#include <include/rbtree.h>

using namespace std;

namespace neweraHPC
{
   void read_headers(rbtree_t *headers, http_data_t *http_data)
   {
      
   }
   
   nhpc_status_t http_content_length(rbtree_t *headers, nhpc_size_t *size)
   {
      for(int i = 1; i <= headers->ret_count(); i++)
      {
	 header_t *header = (header_t *)headers->search(i);

	 if(nhpc_strcmp(header->string, "Content-Length:*") == NHPC_SUCCESS)
	 {
	    string_t *string = nhpc_substr(header->string, ' ');
	    *size = nhpc_strtoi(string->strings[1]);
	    nhpc_string_delete(string);
	    
	    return NHPC_SUCCESS;
	 }
      }

      return NHPC_FAIL;
   }
};