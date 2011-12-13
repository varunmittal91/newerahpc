#ifndef _NETWORK_DATA_H_
#define _NETWORK_DATA_H_

#include <sys/poll.h>

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
