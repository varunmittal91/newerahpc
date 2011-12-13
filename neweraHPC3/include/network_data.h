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

#ifndef _NETWORK_DATA_H_
#define _NETWORK_DATA_H_

#include <sys/poll.h>
#include <netdb.h>

#include "rbtree.h"

namespace neweraHPC
{
   struct nhpc_server_details_t
   {
      pthread_mutex_t *mutex;
      rbtree_t        *client_socks;
   };

   struct nhpc_socket_t
   {
      int sockfd;
      int sockopt;
      int family;
      int incomplete_operation;
      time_t timeout;
      const char *host;
      const char *port;
      addrinfo *hints;
      addrinfo *hints_res;
      rbtree_t *headers;
      bool have_headers;
      nhpc_server_details_t *server_details;
      char *partial_content;
   };
};

#endif
