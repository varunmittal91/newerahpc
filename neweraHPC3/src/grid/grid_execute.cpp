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
#include <unistd.h>
#include <sys/types.h>

#include <include/grid.h>
#include <include/headers.h>
#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t nhpc_grid_server_t::grid_execute(nhpc_instruction_set_t *instruction_set, 
						  nhpc_socket_t *sock, const char **grid_uid)
   {
      char *plugin = instruction_set->plugin_name;
      int *argument_count = &(instruction_set->argument_count);
      rbtree_t *arguments = instruction_set->arguments;
      bool *execute = &(instruction_set->execute);
      char *peer_port = instruction_set->host_peer_port;
      char *peer_host = instruction_set->host_peer_addr;
      rbtree_t *headers = sock->headers;
      
      if(nhpc_strcmp(plugin, GRID_RANGE_PLUGIN) == NHPC_SUCCESS)
      {
	 int start = 0;
	 int end   = 0;
	 char *exec = NULL;
	 char *file = NULL;
	 int range_pos = -1;
	 
	 for(int i = 1; i <= *argument_count; i++)
	 {
	    char *argument = (char *)instruction_set->arguments->search(i);
	    
	    string_t *string = nhpc_substr(argument, ',');
	    int option = nhpc_strtoi(string->strings[0]);
	    
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
		     tmp_str1 = nhpc_strconcat(exec, string->strings[1], " ");
		     delete[] exec;
		     exec = tmp_str1;
		  }
		  break;
		  
	       case COMMAND:
		  exec = nhpc_strconcat(string->strings[1], " ");
		  break;
		  
	       case GRID_FILE:
		  tmp_str1 = nhpc_strconcat(grid_directory, *grid_uid, "/");
		  tmp_str2 = nhpc_strconcat(tmp_str1, string->strings[1], " ");
		  
		  if(exec)
		  {
		     tmp_str3 = nhpc_strconcat(exec, tmp_str2);
		     delete[] exec;
		     exec = tmp_str3;
		  }
		  else 
		     nhpc_strcpy(&exec, tmp_str2);
		  
		  delete[] tmp_str1;
		  delete[] tmp_str2;
		  
		  break;
		  
	       case VALUE:
		  if(exec)
		  {
		     tmp_str1 = nhpc_strconcat(exec, string->strings[1], " ");
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
	       nhpc_instruction_set_t *new_instruction_set;
	       nhpc_create_instruction(&new_instruction_set, GRID_RANGE_PLUGIN, *grid_uid);
	       
	       for(int j = 1; j <= *argument_count; j++)
	       {
		  char *j_str = nhpc_itostr(j);
		  char *argument = nhpc_strconcat("Argument", j_str);
		  delete[] j_str;
		  char *argument_value = (char *)headers->search(argument);
		  delete[] argument;
		  
		  if(j == range_pos)
		     nhpc_add_argument(new_instruction_set, VALUE, &i);
		  else 
		  {
		     char *tmp_str1;
		     nhpc_strcpy(&tmp_str1, argument_value);
		     
		     new_instruction_set->arguments->insert(tmp_str1);
		  }
	       }
	       
	       queue_job(new_instruction_set);
	    }
	    
	    nhpc_delete_instruction(instruction_set);
	    if(exec)
	       delete[] exec;
            return NHPC_SUCCESS;
	 }
	 else 
	 {
	    int rv, pid;
	    nhpc_status_t nrv;
	    nhpc_socket_t *new_sock;
	    
	    char *host_uid = (char *)headers->search("Host-Grid-Uid");
	    char *peer_id = (char *)headers->search("Peer");
	    int peer_id_n = nhpc_strtoi(peer_id);
	    
	    pid = fork();
	    if(pid == 0)
            {
	       system(exec);
	       nrv = socket_connect(&new_sock, sock->host, peer_port, AF_INET, SOCK_STREAM, 0);
	       
	       if(nrv != NHPC_SUCCESS)
	       {
		  nrv = NHPC_FAIL;
		  exit(1);
	       }
	       
	       nhpc_headers_t *headers = new nhpc_headers_t;
	       
	       headers->insert("GRID SUBMISSION 2.90");
	       headers->insert("Grid-Uid", host_uid);
	       headers->insert("Peer", peer_id);
	       nrv = headers->write(new_sock);	       
	       delete headers;
	       
	       if(nrv != NHPC_SUCCESS)
	       {
		  socket_close(new_sock);
		  socket_delete(new_sock);
		  nrv = NHPC_FAIL;
		  exit(1);
	       }	       
	       
               exit(0);
            }
	    else 
	    {
               nhpc_delete_instruction(instruction_set);
            }
	    
	    if(exec)
	       delete[] exec;
	    
   	    return NHPC_SUCCESS;
	 }
      }
      
      return NHPC_FAIL;      
   }
};
