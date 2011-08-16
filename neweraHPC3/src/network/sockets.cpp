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

#include <iostream>

#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t socket_create(nhpc_socket_t **sock, int family, int type, int protocol)
   {
      (*sock)->sockfd = socket(family, type, protocol);
      
      if (((*sock)->sockfd) == -1)
	 return errno;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t socket_create(nhpc_socket_t **sock)
   {
      if(!(*sock))
	 return 0;
      
      addrinfo **hints = &((*sock)->hints);
      if(!(*hints))
	 return 0;
      
      int rv = socket_create(sock, (*hints)->ai_family, (*hints)->ai_socktype, (*hints)->ai_protocol);
      return rv;
   }
   
   nhpc_status_t socket_connect(nhpc_socket_t *sock) 
   {
      int rv;
      
      if (!(sock->hints))
      {
	 return 0;
      }
      
      addrinfo *hints = sock->hints;
      
      do 
      {
	 rv = connect(sock->sockfd, hints->ai_addr, hints->ai_addrlen);
      } while (rv == -1 && errno == EINTR);
      
      if(rv == -1 && (errno == EINPROGRESS || errno == EALREADY) && sock->timeout > 0)
      {
	 rv = nhpc_wait_for_io_or_timeout(sock, 0);
	 if(rv != NHPC_SUCCESS)return rv;
	 else if(rv == NHPC_SUCCESS)
	    rv = connect(sock->sockfd, hints->ai_addr, hints->ai_addrlen);
      }
	 
      if(rv == -1 && errno != EISCONN)
	 return errno;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t socket_open(nhpc_socket_t *sock, int connection_queue)
   {
      int rv;
      int nrv;
      
      int enable_opts = 1;
      
      rv = setsockopt(sock->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable_opts, sizeof(int));
      if(rv == -1)
      {
	 perror("set sock options");
	 return errno;
      }
      
      nrv = socket_bind(sock);
      if(nrv != NHPC_SUCCESS)
      {
	 perror("bind socket");
	 return errno;
      }
      
      nrv = socket_listen(sock, &connection_queue);
      if(nrv != NHPC_SUCCESS)
      {
	 perror("socket listen");
	 return errno;
      }
      
      nrv = socket_accept(sock);
      if(nrv != NHPC_SUCCESS){
	 perror("socket accept");
	 return errno;
      }
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t socket_bind(nhpc_socket_t *sock)
   {
      int rv;
      rv = bind(sock->sockfd, sock->hints->ai_addr, sock->hints->ai_addrlen);
      
      if(rv == -1)
	 return errno;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t socket_listen(nhpc_socket_t *sock, int *connection_queue)
   {
      int rv;
      rv = listen(sock->sockfd, *connection_queue);
      
      if(rv == -1)
	 return errno;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t socket_accept(nhpc_socket_t *sock)
   {
      int rv;
      int nrv;
      
      struct sockaddr_storage client_addr;
      int client_sockfd;
      socklen_t sin_size = sizeof client_addr;
      char s[INET6_ADDRSTRLEN];
      
      while(1)
      {
	 nrv = nhpc_wait_for_io_or_timeout(sock, 1);
	 if(nrv != NHPC_SUCCESS)
	    continue;
	 else
	 {
	    rv = accept(sock->sockfd, (struct sockaddr *)&client_addr, &sin_size);
	    if(rv != -1)
	    {
	       cout<<"connection accepted"<<endl;
	       
	       char buffer[1000];
	       size_t size = 1000;
	       nhpc_socket_t *sock_new = new nhpc_socket_t;
	       sock_new->sockfd = rv;
	       sock_new->timeout = sock->timeout;
	       nrv = 0;
	       int exit_status = 0;
	       while(exit_status != NHPC_EOF)
	       {
		  bzero(buffer, 1000);
		  nrv = socket_recv(sock_new, buffer, &size);
		  cout<<size<<nrv<<endl;
		  cout<<buffer<<endl;		  
		  
		  size_t start_pos = 0;
		  size_t end_pos = 0;
		  
		  for(int cntr = 0; cntr < size; cntr++)
		  {
		     if(buffer[cntr] == '\r')
		     {
			end_pos = cntr;
			size_t length = end_pos - start_pos;
			cout<<length<<" string terminated "<<start_pos<<" "<<end_pos<<endl;
			if(length == 0)
			{
			   exit_status = NHPC_EOF;
			   break;
			}
		     }
		     else if(buffer[cntr] == '\n')
		     {
			cout<<"new line"<<endl;
			start_pos = cntr + 1;
		     }
		     
		  }
	       }
	       
	       const char *buffer_send = "Welcome to NeweraCLuster";
	       size = strlen(buffer_send);
	       socket_send(sock_new, (char *)buffer_send, &size);
	       close(sock_new->sockfd);
	    }
	    else {
	       cout<<"hi"<<endl;
	    }
	 }
      }
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t socket_getaddrinfo(nhpc_socket_t **sock, const char *host_addr, const char *host_port,
				    int family, int type, int protocol) 
   {
      int rv;
      
      addrinfo **hints, **res, *p;
      
      (*sock) = new nhpc_socket_t;
      memset((*sock), 0, sizeof(nhpc_socket_t));
      
      hints = &((*sock)->hints);
      res   = &((*sock)->hints);
      
      (*hints) = new addrinfo;
      memset(*hints, 0, sizeof(addrinfo));
      
      (*hints)->ai_family   = family;
      (*hints)->ai_socktype = type;
      (*hints)->ai_protocol = protocol;
      
      rv = getaddrinfo(host_addr, host_port, *hints, res);
      if (rv == -1)
      {
	 return errno;
      }
      
      return NHPC_SUCCESS;
   }
   
   void *get_in_addr(struct sockaddr *sa)
   {
      if(sa->sa_family == AF_INET) 
      {
	 return &(((struct sockaddr_in*)sa)->sin_addr);
      }
      return &(((struct sockaddr_in6*)sa)->sin6_addr);
   }      
   
   nhpc_status_t socket_delete(nhpc_socket_t *sock)
   {
      freeaddrinfo(sock->hints);
      delete sock;
      
      return NHPC_SUCCESS;
   }
}