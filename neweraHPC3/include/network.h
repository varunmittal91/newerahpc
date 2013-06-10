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

#ifndef _NETWORK_H_
#define _NETWORK_H_

/* This will allow 10 connection to be queued */
#define CONNECTION_QUEUE 10

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

#include "network_data.h"
#include "constants.h"
#include "thread.h"
#include "strings.h"
#include "poll.h"
#include "headers.h"
#include "sockets.h"
#include "sockopts.h"
#include "communication.h"

namespace neweraHPC
{  
   typedef void (*network_addon_ptr_t)(nhpc_socket_t *);
   struct nhpc_network_addon_def_t
   {
      network_addon_ptr_t  fnc;
      const char          *command_str;
   };
   
   struct nhpc_network_t
   {
      bool              external_thread_manager;
      thread_manager_t *thread_manager;
      
      nhpc_mutex_t mutex_general;
      nhpc_mutex_t mutex_addons;
      
      rbtree *client_connections;
      rbtree *network_addons;

      nhpc_socket_t *server_socket;
      const char    *host_addr;
      const char    *host_port;
      
      int accept_thread_id;      
   };
   extern nhpc_network_t *network;
   
#define nhpc_network_set_host_address(a, p)     do{nhpc_strcpy((char **)&(network->host_addr), a); \
						   nhpc_strcpy((char **)&(network->host_port), p); \
						}while(false);
#define nhpc_network_get_thread_manager()	(network->thread_manager)
#define nhpc_network_lock_general(o)		(thread_mutex_lock(&((*network).mutex_general), o))
#define nhpc_network_unlock_general(o)		(thread_mutex_unlock(&((*network).mutex_general), o))
#define nhpc_network_insert_client(s)		do{nhpc_network_lock_general(NHPC_THREAD_LOCK_WRITE);      \
						   (*(*network).client_connections).insert(s, s->sockfd);  \
						   nhpc_network_unlock_general(NHPC_THREAD_LOCK_WRITE);    \
						}while(false);
#define nhpc_network_remove_client(s)		do{nhpc_network_lock_general(NHPC_THREAD_LOCK_WRITE);   \
						   (*(*network).client_connections).erase(s->sockfd);   \
						   nhpc_network_unlock_general(NHPC_THREAD_LOCK_WRITE); \
						}while(false);
#define nhpc_network_spawn_client_thread(s)	do{(*nhpc_network_get_thread_manager()).init_thread(&(s->thread_id), NULL);   \
						   (*nhpc_network_get_thread_manager()).create_thread(&(s->thread_id), NULL,  \
						   (void* (*)(void*))read_communication, s, NHPC_THREAD_DETACH);	      \
						}while(false);
#define nhpc_network_delete_client_thread(s)	((*(*network).thread_manager).delete_thread_data(s->thread_id))
   
#define nhpc_network_lock_addons(o)		(thread_mutex_lock(&((*network).mutex_addons), o))
#define nhpc_network_unlock_addons(o)		(thread_mutex_unlock(&((*network).mutex_addons), o))
#define nhpc_network_insert_addon(d, r)         do{nhpc_network_lock_addons(NHPC_THREAD_LOCK_WRITE);    \
						   r = (*(*network).network_addons).insert(d);          \
						   nhpc_network_unlock_addons(NHPC_THREAD_LOCK_WRITE);  \
						}while(false);
#define nhpc_network_get_addon_def(i)           ((nhpc_network_addon_def_t *)(*(*network).network_addons).search(i))
   
   nhpc_status_t  nhpc_network_init(thread_manager_t *_thread_manager = NULL);   
   nhpc_status_t  nhpc_network_create_server(const char *host_addr, const char *host_port,
		   			     int family, int type, int protocol);
   void          *nhpc_network_accept_thread(void *data);
   void           nhpc_network_join_accept_thread();
   
   nhpc_status_t       nhpc_network_register_addon(const char *command_str, network_addon_ptr_t handler);
   network_addon_ptr_t nhpc_network_find_addon(const char *command_str);
   
   void *get_in_addr(struct sockaddr *sa);   
   void  nhpc_socket_cleanup(nhpc_socket_t *client_sock);
};

#endif
