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
#include <iostream>

using namespace std;

namespace neweraHPC
{
   bool network_t::open_socket(const char *in_host_addr, const char *in_host_port)
   {
      host_addr = (char *)in_host_addr;
      host_port = (char *)in_host_port;
      //////////////////////////
      
      connection_stat = true;
      
      struct addrinfo hints, *servinfo, *p;
      socklen_t sin_size;
      
      struct sigaction sa;
      
      int rv;
      
      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
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
	 (*server_details).sockfd = host_sockfd;
	 (*server_details).main_thread_id = (*thread_manager).create_thread(NULL, connection_handler, 
								       server_details, THREAD_DEFAULT);
      }
      
      return connection_stat;
   }
   void *connection_handler(void *data)
   {
      nhpc_server_details_t *server_details = (nhpc_server_details_t *)data;
      cout<<server_details->main_thread_id<<endl;
      
      struct sockaddr_storage their_addr;
      int new_fd;
      char s[INET6_ADDRSTRLEN];
      
      ///////////////////////////
      while(1) 
      {
	 socklen_t sin_size = sizeof their_addr;
	 new_fd = accept(server_details->sockfd, (struct sockaddr *)&their_addr, &sin_size);
	 if (new_fd == -1) {
            perror("accept");
            continue;
	 }
	 
	 inet_ntop(their_addr.ss_family,
		   get_in_addr((struct sockaddr *)&their_addr),
		   s, sizeof s);
	 printf("server: got connection from %s\n", s);
	 
	 char buffer[1000];
	 recv(new_fd, buffer, 1000, 0);
	 printf(buffer,"%s");
	 
	 if (!fork()) { // this is the child process
            //close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
	       perror("send");
            //close(new_fd);
            exit(0);
	 }
	 int test = (int)close(new_fd);  // parent doesn't need this
      }      
      ///////////////////////////
   }
   void *get_in_addr(struct sockaddr *sa)
   {
      if (sa->sa_family == AF_INET) 
      {
	 return &(((struct sockaddr_in*)sa)->sin_addr);
      }
      return &(((struct sockaddr_in6*)sa)->sin6_addr);
   }   
}