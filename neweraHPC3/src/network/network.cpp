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
      client_connections = new rbtree_t;
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
      
      socket_options_set(server_sock, NHPC_NONBLOCK, 1);
      
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
      accept_thread->sock           = server_sock;
      accept_thread->thread_manager = thread_manager;
      accept_thread->client_socks   = new rbtree_t;
      (*thread_manager).create_thread(NULL, (void * (*)(void *))network_t::accept_connection, (void *)accept_thread, NHPC_THREAD_JOIN);

      return NHPC_SUCCESS;      
   }
   
   void *network_t::accept_connection(nhpc_thread_details_t *main_thread)
   {
      int rv;
      int nrv;
      pthread_mutex_t mutex;
      pthread_mutex_init(&mutex, NULL);
      
      thread_manager_t *thread_manager = main_thread->thread_manager;
      nhpc_socket_t *server_sock = main_thread->sock;
      main_thread->client_socks = new rbtree_t;
      rbtree_t *client_socks = main_thread->client_socks;
      
      struct pollfd fds[200];
      int nfds         = 1;
      int current_size = 0;
      int timeout      = (3 * 60 * 1000);

      int *server_sockfd = &(main_thread->sock->sockfd);
      
      fds[0].fd     = *server_sockfd;
      fds[0].events = POLLIN;
      
      bool end_server = false;
      
      do 
      {
	 cout<<"Waiting on poll()..."<<endl;
	 rv = poll(fds, nfds, timeout);
	 
	 if(rv < 0)
	 {
	    cout<<"\tpoll() failed"<<endl;
	    break;
	 }
	 
	 if(rv == 0)
	 {
	    cout<<"\tpoll() timed out."<<endl;
	    break;
	 }
	 
	 current_size = nfds;
	 for(int cntr = 0; cntr < current_size; cntr++)
	 {
	    if(fds[cntr].revents == 0)
	       continue;
	    
	    if(fds[cntr].revents != POLLIN)
	    {
	       nhpc_socket_t *sock = (nhpc_socket_t *)client_socks->search(fds[cntr].fd);
	       nhpc_sock_cleanup(sock);
	       client_socks->erase(fds[cntr].fd);
	       memset(&fds[cntr], 0, sizeof(pollfd));
	       break;
	    }
	    
	    if(fds[cntr].fd == *server_sockfd)
	    {
	       cout<<"\tListening socket is readable"<<endl;
	       int new_sd;
	       
	       do
	       {
		  new_sd = accept(*server_sockfd, NULL, NULL);
		  if(new_sd < 0)
		  {
		     if(errno != EWOULDBLOCK)
		     {
			end_server = true;
			break;
		     }
		     break;
		  }
		  
		  cout<<"\tNew incoming connection - "<<new_sd<<endl;
		  fds[nfds].fd     = new_sd;
		  fds[nfds].events = POLLIN;
		  nfds++;
		  nhpc_socket_t *client_sock = new nhpc_socket_t;
		  client_sock->sockfd = new_sd;
		  client_sock->headers = NULL;
		  client_socks->insert(client_sock, new_sd);
	       }while(new_sd != -1);
	    }
	    else 
	    {
	       cout<<"\tDescriptor is readable - "<<fds[cntr].fd<<endl;
	       bool close_conn = false;
	       
	       nhpc_socket_t *client_sock = (nhpc_socket_t *)client_socks->search(fds[cntr].fd);
	       //nhpc_socket_t *client_sock = NULL;
	       
	       char buffer[1000];
	       memset(buffer, 0,sizeof(buffer));
	       
	       rv = recv(fds[cntr].fd, buffer, sizeof(buffer), 0);
	       if(rv < 0)
	       {
		  break;
	       }
	       
	       send(fds[cntr].fd, "hi", 2, 0);

	       if(client_sock != NULL)
	       {
		  if(client_sock->headers == NULL)
		     client_sock->headers = new rbtree_t;
		  nhpc_analyze_stream(client_sock->headers, buffer, &rv);
		  nhpc_display_headers(client_sock->headers);
	       }	       
	    }
	 }
	 
      }while(true);
   }
   
   nhpc_status_t nhpc_sock_cleanup(nhpc_socket_t *sock)
   {
      close(sock->sockfd);
      if(sock->headers != NULL)
	 delete sock->headers;
      
      delete sock;
   }
   
   nhpc_status_t nhpc_analyze_stream(rbtree_t *headers, char *data, int *len)
   {
      int line_len = 0;
      int old_pos = 0;
     
      for(int cntr = 0; cntr < *len; cntr++)
      {
	 if(data[cntr] == '\r')
	 {
	    line_len = cntr - old_pos;
	    if(line_len != 0)
	    {
	       char *line = new char [line_len + 1];
	       memcpy(line, (data + old_pos), (line_len));
	       line[line_len] = '\0';

	       header_t *header = new header_t;
	       header->string = line;
	       header->len    = line_len;
	       headers->insert(header);
	    }
	    else 
	       return NHPC_SUCCESS;
	    
	    line_len = 0;
	 }
	 else if(data[cntr] == '\n')
	 {
	    old_pos = cntr + 1;
	 }
      }
      return NHPC_FAIL;
   }
   
   void nhpc_display_headers(rbtree_t *headers)
   {
      if(headers == NULL)return;
      
      header_t *tmp_header;
      
      cout<<"Headers found in the message:"<<(*headers).ret_count()<<endl;
      for(int cntr = 1; cntr <= (*headers).ret_count(); cntr++)
      {
	 tmp_header = (header_t *)(*headers).search(cntr);
	 cout<<"\t"<<tmp_header->string<<endl;
      }
   }

};
