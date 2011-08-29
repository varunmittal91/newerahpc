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

#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   network_t::network_t()
   {
      external_thread_manager = false;
      thread_manager = new thread_manager_t;
      connection_stat = false;
      client_connections = new rbtree;
      mutex = new pthread_mutex_t;
      server_sock = NULL;
   }
   
   network_t::network_t(thread_manager_t *in_thread_manager)
   {
      external_thread_manager = true;
      thread_manager = in_thread_manager;
      connection_stat = false;

      delete client_connections;
      delete mutex;

      if(server_sock != NULL)
	 delete server_sock;
   }
   
   network_t::~network_t()
   {
      if(!external_thread_manager) 
	 delete thread_manager;
   }
   
   inline void network_t::lock()
   {
      pthread_mutex_lock(mutex);
   }
   
   inline void network_t::unlock()
   {
      pthread_mutex_unlock(mutex);
   }
   
   int network_t::add_client_connection(nhpc_socket_t *sock)
   {
      int id;
      lock();
      id = (*client_connections).insert(sock);
      unlock();
      
      return id;
   }
   
   nhpc_status_t network_t::connect(nhpc_socket_t **sock, const char *host_addr, 
				    const char *host_port, int family, int type, int protocol)
   {
      int nrv;
      
      *sock = new nhpc_socket_t;
      
      nrv = socket_getaddrinfo(sock, host_addr, host_port, family, type, protocol);
      if(nrv != NHPC_SUCCESS)
      {
	 delete *sock;
	 return nrv;
      }
      
      nrv = socket_create(sock);     
      if(nrv != NHPC_SUCCESS)
      {
	 delete *sock;
	 return nrv;
      }   
      
      socket_options_set(*sock, NHPC_NONBLOCK, 1);
      
      nrv = socket_connect(*sock);
      if(nrv != NHPC_SUCCESS)
      {
	 delete *sock;
	 return nrv;
      }      

      add_client_connection(*sock);
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t network_t::create_server(const char *host_addr, const char *host_port,
					  int family, int type, int protocol)
   {            
      int rv, nrv;
      int connection_queue = 0;
      
      int enable_opts = 1;
      
      if(server_sock == NULL)
	 server_sock = new nhpc_socket_t;
      
      nrv = socket_getaddrinfo(&server_sock, host_addr, host_port, family, type, protocol);
      if(nrv != NHPC_SUCCESS)
      {
	 delete server_sock;
	 server_sock = NULL;
	 return nrv;
      }      
      
      nrv = socket_create(&server_sock);     
      if(nrv != NHPC_SUCCESS)
      {
	 delete server_sock;
	 return nrv;
      }    
      
      //socket_options_set(server_sock, NHPC_NONBLOCK, 1);
      
      rv = setsockopt(server_sock->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable_opts, sizeof(int));
      if(rv == -1)
      {
	 delete server_sock;
	 server_sock = NULL;
	 return errno;
      }
      
      nrv = socket_bind(server_sock);
      if(nrv != NHPC_SUCCESS)
      {
	 delete server_sock;
	 server_sock = NULL;
	 return errno;
      }
      
      nrv = socket_listen(server_sock, &connection_queue);
      if(nrv != NHPC_SUCCESS)
      {
	 delete server_sock;
	 server_sock = NULL;
	 return errno;
      }
      
      nhpc_thread_details_t *accept_thread = new nhpc_thread_details_t;
      accept_thread->sock = server_sock;
      accept_thread->thread_manager = thread_manager;
      (*thread_manager).create_thread(NULL, (void * (*)(void *))network_t::accept_connection, (void *)accept_thread, NHPC_THREAD_JOIN);
            
      return NHPC_SUCCESS;      
   }
   
   void *network_t::accept_connection(nhpc_thread_details_t *main_thread)
   {
      int rv;
      int nrv;
      
      thread_manager_t *thread_manager = main_thread->thread_manager;
      nhpc_socket_t *server_sock = main_thread->sock;

      struct sockaddr_storage *client_addr;
      socklen_t sin_size;
      
      while(1)
      {
	 client_addr = new sockaddr_storage;
	 sin_size = sizeof(*client_addr);

	 bzero(&client_addr, sizeof(sockaddr_storage));
	 rv = accept(server_sock->sockfd, (struct sockaddr *)&client_addr, &sin_size);
	 if(rv < 0)
	 {
	    perror("error at accept");
	    sleep(1);
	 }
	 else	
	 {
	    nhpc_socket_t *client_sock = new nhpc_socket_t;
	    client_sock->sockfd = rv;
	    cout<<"Connection accepted";
	    (*thread_manager).create_thread(NULL, (void * (*)(void *))network_t::connection_handler, (void *)client_sock, 0);
	    cout<<"thread created"<<endl;
	 }
      }
      cout<<"hello guys"<<endl;
   }
   
   void *network_t::connection_handler(nhpc_socket_t *sock)
   {
      cout<<"hi in connection"<<endl;

      char buffer[1000];
      size_t size = 1000;
      socket_recv(sock, buffer, &size);
      cout<<buffer<<endl;
      
      socket_send(sock, buffer, &size);
      
      close(sock->sockfd);
      delete sock;
   }
   
   int test_socket_factory()
   {
      int sockfd, newsockfd, portno;
      socklen_t clilen;
      char buffer[256];
      struct sockaddr_in serv_addr, cli_addr;
      int n;

      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) 
	 perror("ERROR opening socket");
      bzero((char *) &serv_addr, sizeof(serv_addr));
      portno = atoi("8080");
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_port = htons(portno);
      if (bind(sockfd, (struct sockaddr *) &serv_addr,
	       sizeof(serv_addr)) < 0) 
	 perror("ERROR on binding");
      listen(sockfd,5);
      while(1)
      {
	 clilen = sizeof(cli_addr);
	 newsockfd = accept(sockfd, 
			    (struct sockaddr *) &cli_addr, 
			    &clilen);
	 if (newsockfd < 0) 
	    perror("ERROR on accept");
	 bzero(buffer,256);
	 n = read(newsockfd,buffer,255);
	 if (n < 0) perror("ERROR reading from socket");
	 printf("Here is the message: %s\n",buffer);
	 n = write(newsockfd,"I got your message",18);
	 if (n < 0) perror("ERROR writing to socket");
	 close(newsockfd);
	 close(sockfd);
      }
      return 0; 
   }      
};
