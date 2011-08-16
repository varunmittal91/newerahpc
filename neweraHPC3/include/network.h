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

#include "constants.h"
#include "thread.h"
#include "strings.h"
#include "poll.h"
#include "headers.h"
#include "sockets.h"
#include "sockopts.h"

namespace neweraHPC
{   
   struct nhpc_thread_details_t
   {
      int sockfd;
      int main_thread_id;
      thread_manager_t *thread_manager;
      nhpc_socket_t *socket;
   };
            
   void *connection_handler(void *data);
   
   void *connection_thread(void *data);
   
   void *get_in_addr(struct sockaddr *sa);
   
   class network_t
   {
   private:
      char *host_addr;
      char *host_port;
      int  host_port_network_byte;
      int  host_sockfd;
      bool connection_stat;
      thread_manager_t *thread_manager;
      bool external_thread_manager;
      struct nhpc_thrad_details_t *server_thread_details;
      
   public:
      network_t();
      network_t(thread_manager_t *in_thread_manager);
      ~network_t();
      nhpc_status_t connect(nhpc_socket_t **sock, const char *host_addr, 
			    const char *host_port, int family, int type, int protocol);
      nhpc_status_t create_server();
   };
   
   nhpc_status_t nhpc_recv(nhpc_socket_t *sock, char *buffer, size_t *len);
   nhpc_status_t nhpc_analyze_stream(char *buffer, size_t *len);
};

#endif