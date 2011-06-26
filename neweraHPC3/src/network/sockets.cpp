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

/* These routines are usefull for creating listening sockets(opening sockets)
 or connecting to other machines on given port */

#include <include/network.h>
#include <errno.h>
#include <iostream>

using namespace std;

namespace neweraHPC
{
   bool network_t::open_socket(const char *in_host_addr, const char *in_host_port)
   {
      host_addr = (char *)in_host_addr;
      host_port = (char *)in_host_port;
      
      connection_stat = true;
      
      struct addrinfo hints, *servinfo, *p;
      
      int rv;
      
      memset(&hints, 0, sizeof hints);
      
      /* If IPv4_ONLY is set in the configure script than only IPv4
       addresses will be used otherwise both IPv4 and IPv6 */
#ifdef IPv4_ONLY
      hints.ai_family = AF_INET;
#else
      hints.ai_family = AF_UNSPEC;
#endif
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE; // use my IP
      
      int enable_opts = 1;
      
      if ((rv = getaddrinfo(NULL, host_port, &hints, &servinfo)) != 0) {
	 fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      }
      
      /* Loop for creating socket, setting socket options
       and bind to the address */
      for(p = servinfo; p != NULL; p = p->ai_next) 
      {
	 if ((host_sockfd = socket(p->ai_family, p->ai_socktype,
				   p->ai_protocol)) == -1)
	 {
	    perror("server: socket");
	    connection_stat = false;
	    continue;
	 }
	 
	 if (setsockopt(host_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable_opts,
			sizeof(int)) == -1) 
	 {
	    perror("setsockopt");
	    connection_stat = false;
	 }
	 
	 if (bind(host_sockfd, p->ai_addr, p->ai_addrlen) == -1) 
	 {
	    close(host_sockfd);
	    perror("server: bind");
	    connection_stat = false;
	    continue;
	 }
	 
	 if (p == NULL)  {
	    fprintf(stderr, "server: failed to bind\n");
	 }
	 
	 break;
      }
      
      freeaddrinfo(servinfo);
      
      if (listen(host_sockfd, CONNECTION_QUEUE) == -1) {
	 perror("listen");
      }
      
      if(connection_stat)
      {
	 server_details = new nhpc_server_details_t;
	 (*server_details).thread_manager = thread_manager;
	 (*server_details).sockfd	  = host_sockfd;
	 (*server_details).main_thread_id = (*thread_manager).create_thread(NULL, connection_handler, 
									    server_details, NHPC_THREAD_DEFAULT);
      }
      
      return connection_stat;
   }
   
   void *connection_handler(void *data)
   {
      nhpc_server_details_t *server_details = (nhpc_server_details_t *)data;
      thread_manager_t *thread_manager      = (*server_details).thread_manager;
      
      struct sockaddr_storage their_addr;
      int client_sockfd;
      char s[INET6_ADDRSTRLEN];
      
      while(1) 
      {
	 socklen_t sin_size = sizeof their_addr;
	 client_sockfd = accept(server_details->sockfd, (struct sockaddr *)&their_addr, &sin_size);
	 if (client_sockfd == -1) {
            perror("accept");
            continue;
	 }
	 
	 inet_ntop(their_addr.ss_family,
		   get_in_addr((struct sockaddr *)&their_addr),
		   s, sizeof s);
	 	 
	 nhpc_client_details_t *client_details = new nhpc_client_details_t;
	 (*client_details).thread_manager = thread_manager;
	 (*client_details).sockfd	  = client_sockfd;
	 (*client_details).thread_id	  = (*thread_manager).create_thread(NULL, connection_thread,
									    client_details, NHPC_THREAD_DEFAULT);								    
      }     
   }
   
   void *connection_thread(void *data)
   {
      nhpc_client_details_t *client_details = (nhpc_client_details_t *)data;
      thread_manager_t *thread_manager = (*client_details).thread_manager;

      nhpc_socket_t *sock = new nhpc_socket_t;
      sock->sockfd  = client_details->sockfd;
      sock->timeout = 0;
      sock->options |= NHPC_SUCCESS;
      
      size_t len = 500;
      char *buffer = new char [len];
      int nrv = nhpc_recv(sock, buffer, &len);
      if(nrv == ECONNRESET)
      {
	 perror("nhpc_recv");
	 goto connection_close;
      }
      
      if(nrv & NHPC_SUCCESS)
      {
	 nhpc_analyze_stream(buffer, &len);
      }
      
      if(send(client_details->sockfd, "Hello, world!", 13, 0) < 0)
	 perror("Send");

   connection_close:
      (*thread_manager).delete_thread_data((*client_details).thread_id);
      close((*client_details).sockfd);
      delete (client_details);
      delete sock;
      delete buffer;
      pthread_exit(NULL);
   }
	     
   void *get_in_addr(struct sockaddr *sa)
   {
      if(sa->sa_family == AF_INET) 
      {
	 return &(((struct sockaddr_in*)sa)->sin_addr);
      }
      return &(((struct sockaddr_in6*)sa)->sin6_addr);
   }   
}