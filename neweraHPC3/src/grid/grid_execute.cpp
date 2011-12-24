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

#include <include/grid.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t nhpc_grid_server_t::grid_execute(nhpc_socket_t *sock, const char **grid_uid)
   {
      rbtree_t *headers = sock->headers;
      
      char *plugin = (char *)headers->search("Plugin");
      char *argument_count_str = (char *)headers->search("Argument-Count");
      int argument_count = nhpc_strtoi(argument_count_str);
      
      if(!plugin || !argument_count_str)
	 return NHPC_FAIL;
      
      if(nhpc_strcmp(plugin, GRID_RANGE_PLUGIN) == NHPC_SUCCESS)
      {
	 for(int i = 1; i <= argument_count; i++)
	 {
	    char *argument_str = nhpc_strconcat("Argument", nhpc_itostr(i));
	    char *argument = (char *)headers->search(argument_str);
	    delete[] argument_str;
	    if(!argument)
	       return NHPC_FAIL;
	    
	    string_t *string = nhpc_substr(argument, ',');
	    int option = nhpc_strtoi(argument);
	    char *exec = NULL;
	    
	    switch(option)
	    {
	       case RANGE:
		  break;
	       case LITERAL:
		  break;
	       case COMMAND:
		  break;
	       case GRID_FILE:
		  break;
	    };
	    
	    nhpc_string_delete(string);
	 }
      }
      
      return NHPC_FAIL;
   }  
};