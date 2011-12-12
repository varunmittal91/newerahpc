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
   
   int network_t::add_client_connection(nhpc_socket_t *sock, int sockfd)
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
      nhpc_status_t nrv;
      
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
      
      (*sock)->headers = new rbtree_t;
      add_client_connection(*sock, (*sock)->sockfd);
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
   
   void sig_action(int sig)
   {
      if(sig == SIGPIPE)
	 cout<<"PIPE Broken at thread - "<<pthread_self()<<endl;
   }
   
   void *network_t::accept_connection(nhpc_thread_details_t *main_thread)
   {
      nhpc_size_t rv;
      nhpc_status_t nrv;
      pthread_mutex_t mutex;
      pthread_mutex_init(&mutex, NULL);
      
      thread_manager_t *thread_manager = main_thread->thread_manager;
      nhpc_socket_t *server_sock = main_thread->sock;
      main_thread->client_socks = new rbtree_t;
      rbtree_t *client_socks = main_thread->client_socks;
      
      int poll_limit   = 200;
      struct pollfd *fds = new pollfd[200];
      int nfds           = 1;
      int current_size   = 1;
      int timeout        = (3 * 60 * 1000);
      
      nhpc_server_details_t *server_details = new nhpc_server_details_t;
      server_details->fds          = fds;
      server_details->nfds         = &nfds;
      server_details->mutex        = &mutex;
      server_details->client_socks = client_socks;
      
      int *server_sockfd = &(main_thread->sock->sockfd);
      
      fds[0].fd     = *server_sockfd;
      fds[0].events = POLLIN;
      
      bool end_server = false;
      
      do 
      {
	 rv = poll(fds, nfds, timeout);
	 
	 if(rv < 0)
	 {
	    break;
	 }
	 
	 if(rv == 0)
	 {
	    continue;
	 }
	 
	 current_size = nfds;
	 for(int cntr = 0; cntr < current_size; cntr++)
	 {
	    bool close_conn = false;
	    
	    if(fds[cntr].revents == 0)
	       continue;
	    
	    nhpc_socket_t *client_sock = (nhpc_socket_t *)client_socks->search(fds[cntr].fd);
	    
	    if(fds[cntr].revents != POLLIN)
	    {
	       nhpc_socket_cleanup(client_sock, client_socks, fds, cntr, &nfds);

	       pthread_mutex_lock(&mutex);
	       nhpc_poll_clean(fds, &nfds, &cntr);
	       pthread_mutex_unlock(&mutex);
	       break;
	    }
	    else 
	    {
	       fds[cntr].revents = 0;
	       
	       if(cntr != 0)
	       fds[cntr].events = 0;
	    }
	    
	    if(fds[cntr].fd == *server_sockfd)
	    {
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
		  
		  fds[nfds].fd     = new_sd;
		  fds[nfds].events = POLLIN;
		  nfds++;
		  nhpc_socket_t *client_sock = new nhpc_socket_t;
		  client_sock->sockfd = new_sd;
		  client_sock->headers = NULL;
		  client_sock->have_headers = false;
	  	  client_sock->server_details = server_details;
		  client_sock->timeout = 3 * 60 * 60;

                  pthread_mutex_lock(&mutex);
		  client_socks->insert(client_sock, new_sd);
                  pthread_mutex_unlock(&mutex);
	       }while(new_sd != -1);
	    }
	    else 
	    {
	       client_sock->fds_pos = cntr;
	       thread_manager->create_thread(NULL, (void* (*)(void*))read_communication, client_sock, NHPC_THREAD_DEFAULT);
	    }	    
	 }
      }while(true);
   }
   
   void nhpc_socket_cleanup(nhpc_socket_t *client_sock, rbtree_t *client_socks, pollfd *fds, int cntr, int *nfds)
   {
      if(client_sock != NULL)
      {
	 if(client_sock->headers != NULL)
	 {
	    int count = client_sock->headers->ret_count();
	    for(int i = 1; i <= count; i++)
	    {	
	       header_t *header = (header_t *)client_sock->headers->search(i);
	       delete[] header->string;
	       delete header;
	       client_sock->headers->erase(i);
	    }
	    
	    delete client_sock->headers;
	 }
	 client_socks->erase(fds[cntr].fd);
	 delete client_sock;
      }
      
      shutdown(fds[cntr].fd, SHUT_RDWR);
      close(fds[cntr].fd);
      
      memset((fds + (cntr)), 0, sizeof(pollfd));
   }
   
   void nhpc_poll_clean(pollfd *fds, int *nfds, int *cntr)
   {
      for(int i = *cntr; i < *nfds; i++)
      {	 
	 if(fds[i].fd == 0 && fds[i + 1].fd != 0)
	 {
	    memcpy((fds + i), (fds + i + 1), sizeof(pollfd));
	    memset((fds + i + 1), 0, sizeof(pollfd));
	    (*nfds)--;
	 }
      }      
   }
   
   nhpc_status_t nhpc_analyze_stream(nhpc_socket_t *sock, char *data, nhpc_size_t *len, nhpc_size_t *header_size)
   {
      if(header_size != NULL)
	 *header_size = *len;
      
      if(sock->have_headers == true)
	 return NHPC_SUCCESS;
      
      int line_len = 0;
      int old_pos = 0;
      rbtree_t *headers = sock->headers;
      
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
	    {
	       if(header_size != NULL)
	       {
		  (*header_size) = (*len) - (cntr + 1);
		  if(data[cntr + 1] == '\n')
		     (*header_size)--;
	       }
	       
	       sock->have_headers = true;
	       return NHPC_SUCCESS;
	    }
	    
	    line_len = 0;
	 }
	 else if(data[cntr] == '\n')
	 {
	    old_pos = cntr + 1;
	 }
      }
      return NHPC_FAIL;
   }
   
   void nhpc_display_headers(nhpc_socket_t *sock)
   {
      rbtree_t *headers = sock->headers;
      
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
