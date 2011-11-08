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
      accept_thread->sock = server_sock;
      accept_thread->thread_manager = thread_manager;
      (*thread_manager).create_thread(NULL, (void * (*)(void *))network_t::accept_connection, (void *)accept_thread, NHPC_THREAD_JOIN);

      return NHPC_SUCCESS;      
   }
   
   static void test_signal_hndlr(int signal)
   {
      printf("Received a signal from the accept thread\n");
   }
   
   void *network_t::accept_connection(nhpc_thread_details_t *main_thread)
   {
      signal(SIGPIPE, test_signal_hndlr);
      
      int rv;
      int nrv;
      
      thread_manager_t *thread_manager = main_thread->thread_manager;
      nhpc_socket_t *server_sock = main_thread->sock;

      struct sockaddr_storage *client_addr;
      socklen_t sin_size;
      rbtree_t *headers = NULL;
      
      //////Select data types
      int max_sd, desc_ready;
      int new_sd;
      bool end_server = false, close_conn;
      fd_set master_set, working_set;
      FD_ZERO(&master_set);
      max_sd = server_sock->sockfd;
      FD_SET(server_sock->sockfd, &master_set);
      
      do 
      {
	 memcpy(&working_set, &master_set, sizeof(master_set));	 
	 
	 printf("Waiting on select()...\n");
	 
	 rv = select(max_sd + 1, &working_set, NULL, NULL, NULL);
	 if(rv < 0)
	 {
	    perror("Select failed \n");
	    break;
	 }
	 
	 desc_ready = rv;
	 
	 for (int i=0; i <= max_sd && desc_ready > 0; ++i)
	 {
	    if (FD_ISSET(i, &working_set))
	    {
	       desc_ready -= 1;
	       
	       if (i == server_sock->sockfd)
	       {
		  printf("  Listening socket is readable\n");
		  do
		  {
		     new_sd = accept(server_sock->sockfd, NULL, NULL);
		     if (new_sd < 0)
		     {
			if (errno != EWOULDBLOCK)
			{
			   perror("  accept() failed");
			   end_server = true;
			}
			break;
		     }

		     printf("  New incoming connection - %d\n", new_sd);
		     FD_SET(new_sd, &master_set);
		     if (new_sd > max_sd)
			max_sd = new_sd;
		     
		  } while (new_sd != -1);
	       }
	       else
	       {
		  printf("  Descriptor %d is readable\n", i);
		  close_conn = false;
		  do
		  {
		     char buffer[1000];
		     bzero(&buffer, 1000);
		     
		     rv = recv(i, buffer, sizeof(buffer), 0);
		     if (rv < 0)
		     {
			if (errno != EWOULDBLOCK)
			{
			   close_conn = true;
			}
			break;
		     }
		     else if (rv == 0)
		     {
			printf("  Connection closed\n");
			close_conn = true;
			printf("breaking off at 0\n");
			break;
		     }
		     else 
		     {
			cout<<"  length "<<rv<<endl;
			if(nhpc_analyze_stream(&headers, buffer, &rv) == NHPC_SUCCESS)
			{
			   close_conn = true;
			   break;
			}
		     }
		     
		     int len = rv;		     
		     printf("  %d bytes received\n", len);
		     
		  }while(true);
		  
		  if (close_conn)
		  {
		     printf("closing connection\n");
		     send(i, (const char *)"NeweraHPC\n", 11, 0);
		     close(i);
		     FD_CLR(i, &master_set);
		     if (i == max_sd)
		     {
			while (FD_ISSET(max_sd, &master_set) == false)
			   max_sd -= 1;
		     }
		  }
	       }
	    }
	 }
      }while(1);      
   }
   
   nhpc_status_t nhpc_analyze_stream(rbtree_t **headers, char *data, int *len)
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
	       
	       if(*headers == NULL)
		  *headers = new rbtree_t;
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

};
