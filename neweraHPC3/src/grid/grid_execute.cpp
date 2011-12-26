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
#include <include/headers.h>
#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t nhpc_grid_server_t::grid_execute(nhpc_socket_t *sock, const char **grid_uid)
   {
      rbtree_t *headers = sock->headers;
      
      char *plugin = (char *)headers->search("Plugin");
      char *argument_count_str = (char *)headers->search("Argument-Count");
      int argument_count = nhpc_strtoi(argument_count_str);
      char *execution_state = (char *)headers->search("Execution-State");
      char *peer_id_str = (char *)headers->search("Peer");
      
      bool execute;
      if(execution_state)
	 execute = true;
      else 
	 execute = false;
      
      if(!plugin || !argument_count_str)
	 return NHPC_FAIL;
      
      if(nhpc_strcmp(plugin, GRID_RANGE_PLUGIN) == NHPC_SUCCESS)
      {
	 int start = 0;
	 int end   = 0;
	 char *exec = NULL;
	 char *file = NULL;
	 int range_pos = -1;
	 
	 for(int i = 1; i <= argument_count; i++)
	 {
	    char *argument_str = nhpc_strconcat("Argument", nhpc_itostr(i));
	    char *argument = (char *)headers->search(argument_str);
	    delete[] argument_str;
	    if(!argument)
	       return NHPC_FAIL;
	    
	    string_t *string = nhpc_substr(argument, ',');
	    int option = nhpc_strtoi(string->strings[0]);
	    peer_details_t *peer_details;
	    
	    char *tmp_str1;
	    char *tmp_str2;
	    char *tmp_str3;
	    char *tmp_str4;
	    
	    switch(option)
	    {
	       case RANGE:
		  start = nhpc_strtoi(string->strings[1]);
		  end   = nhpc_strtoi(string->strings[2]);
		  execute = false;
		  range_pos = i;
		  
		  break;
	       case LITERAL:
		  if(exec)
		  {
		     tmp_str1 = nhpc_strconcat(exec, string->strings[1]);
		     tmp_str2 = nhpc_strconcat(tmp_str1, " ");
		     exec = tmp_str2;
		     
		     delete[] tmp_str1;
		  }
		  
		  break;
	       case COMMAND:
		  exec = nhpc_strconcat(string->strings[1], " ");
		  break;
	       case GRID_FILE:
		  tmp_str1 = nhpc_strconcat(grid_directory, *grid_uid);
		  tmp_str2 = nhpc_strconcat(tmp_str1, "/");
		  tmp_str3 = nhpc_strconcat(tmp_str2, string->strings[1]);
		  
		  if(exec)
		  {
		     tmp_str4 = nhpc_strconcat(exec, tmp_str3);
		     delete[] exec;
		     exec = tmp_str4;
		  }
		  else 
		     nhpc_strcpy(&exec, tmp_str3);
		  		  
		  delete[] tmp_str1;
		  delete[] tmp_str2;
		  delete[] tmp_str3;
		  
		  tmp_str1 = nhpc_strconcat(exec, " ");
		  delete[] exec;
		  exec = tmp_str1;

		  break;
	       case VALUE:
		  if(exec)
		  {
		     tmp_str1 = nhpc_strconcat(exec, " ", string->strings[1]);
		     delete[] exec;
		     exec = tmp_str1;
		  }
		  break;
	    };
	    
	    nhpc_string_delete(string);
	 }
	 	 
	 if(!execute)
	 {
	    for(int i = start; i <= end; i++)
	    {
	       nhpc_instruction_set_t *instruction_set;
	       nhpc_create_instruction(&instruction_set, GRID_RANGE_PLUGIN);
	       
	       for(int j = 1; j <= argument_count; j++)
	       {
		  char *argument = nhpc_strconcat("Argument", nhpc_itostr(j));
		  char *argument_value = (char *)headers->search(argument);
		  delete[] argument;
		  
		  if(j == range_pos)
		     nhpc_add_argument(instruction_set, VALUE, &i);
		  else 
		  {
		     instruction_set->arguments->insert(argument_value);
		  }
	       }
	       
	       queue_job(instruction_set, *grid_uid);
	    }
	 }
	 else 
	 {
	    int rv = system(exec);
	    char *host_uid = (char *)headers->search("Host-Grid-Uid");
	    char *peer_id = (char *)headers->search("Peer");
	    int peer_id_n = nhpc_strtoi(peer_id);
   
	    if(peer_id_n == 1)
	    {
	       free_peer(1);
	       return NHPC_SUCCESS;
	    }
	    
	    nhpc_socket_t *new_sock;
	    
	    nhpc_status_t nrv = socket_connect(&new_sock, sock->host, "8080", AF_INET, SOCK_STREAM, 0);
	    
	    if(peer_id_n == 1)
	    {
	       cout<<sock->host<<endl;
	    }
	    
	    if(nrv != NHPC_SUCCESS)
	    {
	       socket_delete(new_sock);
	       
	       if(peer_id_n == 1)
		  exit(1);
	       
	       return NHPC_FAIL;
	    }
	    
	    nhpc_headers_t *headers = new nhpc_headers_t;
	    headers->insert("GRID SUBMISSION 2.90");
	    headers->insert("Grid-Uid", host_uid);
	    headers->insert("Peer", peer_id);
	    cout<<sock->host<<endl;
	    nrv = headers->write(new_sock);
	    if(nrv != NHPC_SUCCESS)
	    
	    return NHPC_SUCCESS;
	 }
      }
      
      return NHPC_FAIL;
   }  
};