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

#include <errno.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/network.h>
#include <include/error.h>
#include <include/neweraHPC.h>

using namespace std;

namespace neweraHPC
{
   nhpc_network_t *network;

   nhpc_status_t nhpc_network_init(thread_manager_t *_thread_manager)
   {
      network = new nhpc_network_t;
      memset(network, 0, sizeof(nhpc_network_t));
      
      (*network).client_connections = new rbtree;
      (*network).network_addons     = new rbtree;
      
      thread_mutex_init(&((*network).mutex_general));
      thread_mutex_init(&((*network).mutex_addons));      
      
      if(_thread_manager)
      {
	 (*network).thread_manager          = _thread_manager;
	 (*network).external_thread_manager = true;
      }
      else 
	 (*network).thread_manager = new thread_manager_t;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_network_create_server(const char *host_addr, const char *host_port,
					    int family, int type, int protocol)
   {
      nhpc_status_t   nrv;
      nhpc_socket_t **server_socket = &((*network).server_socket);

      int connection_queue = CONNECTION_QUEUE;
      int enable_opts      = 1;
      int rv;
      
      socket_init(server_socket);

      if((nrv = socket_getaddrinfo(server_socket, host_addr, host_port, family, type, protocol)) != NHPC_SUCCESS)
	 goto return_status;
	 
      if((nrv = socket_create(server_socket)) != NHPC_SUCCESS)
	 goto return_status;
	 
      socket_options_set((*server_socket), NHPC_NONBLOCK, 1);
      
      if((rv = setsockopt((*server_socket)->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable_opts, sizeof(int))) == -1)
      {
	 nrv = errno;
	 goto return_status;
      }
      
      if((nrv = socket_bind(*server_socket)) != NHPC_SUCCESS)
	 goto return_status;
      
      if((nrv = socket_listen(*server_socket, &connection_queue)) != NHPC_SUCCESS)
	 goto return_status;
      
   return_status:
      if(nrv != NHPC_SUCCESS)
      {
	 perror("ERR: nhpc_create_server");
	 
	 delete (*server_socket);
	 (*server_socket) = NULL;
      }
      else 
      {
	 nhpc_network_set_host_address(host_addr, host_port);
      }
      
      (*(*network).thread_manager).init_thread(&((*network).accept_thread_id), NULL);
      (*(*network).thread_manager).create_thread(&((*network).accept_thread_id), NULL, nhpc_network_accept_thread, 
						 NULL, NHPC_THREAD_DEFAULT);
      
      return nrv;
   }
   
   void *nhpc_network_accept_thread(void *data)
   {
      nhpc_size_t       size;
      nhpc_status_t     nrv;
      nhpc_socket_t    *server_socket  = (*network).server_socket;
      thread_manager_t *thread_manager = (*network).thread_manager;
      
      int          new_sockfd;
      sockaddr_in  client_sockaddr;

      bool end_server = false;
      
      do 
      {
	 if((nrv = nhpc_wait_for_io_or_timeout(server_socket, 1)) != NHPC_SUCCESS)
	 {
	    continue;
	 }
	 
	 do 
	 {
	    size            = sizeof(sockaddr_in);
	    memset(&client_sockaddr, 0, sizeof(sockaddr_in));
	    
	    new_sockfd = accept((*server_socket).sockfd, (sockaddr *)&client_sockaddr, (socklen_t *)&size);
	    if(new_sockfd < 0)
	    {
	       if(errno != EWOULDBLOCK)
		  end_server = true;
	       
	       break;
	    }
	    
	    nhpc_socket_t *new_socket;
	    socket_init(&new_socket);
	    new_socket->sockfd = new_sockfd;
	    
	    char *tmp_port = nhpc_itostr(ntohs(client_sockaddr.sin_port));
	    nhpc_strcpy_noalloc((new_socket->port), tmp_port);
	    nhpc_strcpy_noalloc((new_socket->host), inet_ntoa(client_sockaddr.sin_addr));
	    nhpc_string_delete(tmp_port);
	    
	    nhpc_network_insert_client(new_socket);
	    nhpc_network_spawn_client_thread(new_socket);
	 }while(new_sockfd != -1);
      }while(!end_server);
   }
   
   void nhpc_network_join_accept_thread()
   {
      (*(*network).thread_manager).join_thread((*network).accept_thread_id);
   }
   
   nhpc_status_t nhpc_network_register_addon(const char *command_str, network_addon_ptr_t fnc)
   {
      int rv;
      
      nhpc_network_addon_def_t *addon_def = new nhpc_network_addon_def_t;
      (*addon_def).command_str = command_str;
      (*addon_def).fnc         = fnc;
      
      nhpc_network_insert_addon(addon_def, rv);
      
      if(!rv)
      {
	 delete addon_def;
	 return NHPC_FAIL;
      }
      
      return NHPC_SUCCESS;
   }
   
   network_addon_ptr_t nhpc_network_find_addon(const char *command_str)
   {
      nhpc_network_lock_addons(NHPC_THREAD_LOCK_READ);
      
      nhpc_network_addon_def_t *addon_def;

      int i = 1;
      while(((addon_def = nhpc_network_get_addon_def(i)) && (nhpc_strcmp(command_str, addon_def->command_str) != NHPC_SUCCESS)))
	 i++;

      nhpc_network_unlock_addons(NHPC_THREAD_LOCK_WRITE);
      
      if(addon_def)
	 return (addon_def->fnc);

      return NULL;
   }
   
   void nhpc_socket_cleanup(nhpc_socket_t *client_socket)
   {    
      int *sockfd = &((*client_socket).sockfd);
      
      nhpc_network_remove_client(client_socket);	 
      nhpc_network_delete_client_thread(client_socket);
	 
      socket_close(client_socket);
      socket_delete(client_socket);
   }
};
