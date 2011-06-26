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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#include "constants.h"
#include "thread.h"
#include "strings.h"
#include "poll.h"
#include "network/headers.h"

namespace neweraHPC
{
   /* General socket related data and options */
   struct nhpc_socket_t
   {
      int sockfd;
      int options;
      time_t timeout;
   };   
   
   /* Local structure for sending data to the connection_handlre */
   struct nhpc_server_details_t
   {
      int sockfd;
      int main_thread_id;
      thread_manager_t *thread_manager;
   };
   
   /* Local structure for sending data to the connection_thread */
   struct nhpc_client_details_t
   {
      int sockfd;
      int thread_id;
      thread_manager_t *thread_manager;
      nhpc_socket_t *socket;
   };
      
   /* Routine for handling tcp connections, *data is a nhpc_server_details_t 
    type which gives basic details to the thread */
   void *connection_handler(void *data);
   
   /* Routine for handling indivisual connection request */
   void *connection_thread(void *data);
   
   /* Return sockaddr according to ipv4 or ipv6 type */
   void *get_in_addr(struct sockaddr *sa);
   
   class network_t
   {
   private:
      char *host_addr;
      char *host_port;
      int  host_port_network_byte;
      int  host_sockfd;
      /* Connection_stat will be set to true only
       if the socket is created and bind complete */
      bool connection_stat;
      /* Thread manager routine defined in thread.h, 
       it can be created inside the class or the address 
       of the external can be given */
      thread_manager_t *thread_manager;
      bool external_thread_manager;
      struct nhpc_server_details_t *server_details;
      
   public:
      network_t();
      /* For supplying the address of an external thread_manager_t */
      network_t(thread_manager_t *in_thread_manager);
      ~network_t();
      /* Return status of the connection */ 
      bool status();
      bool open_socket(const char *in_host_addr, const char *in_host_port);
   };
   
   /* Send and receive routines and variables */
   nhpc_status_t nhpc_recv(nhpc_socket_t *sock, char *buffer, size_t *len);
   nhpc_status_t nhpc_analyze_stream(char *buffer, size_t *len);
};

#endif