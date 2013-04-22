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

#ifndef _HTTP_H_
#define _HTTP_H_

#include "constants.h"
#include "network_data.h"

#define HTTP_ROOT "/www"

enum request_type
{
   HTTP_INVALID,
   HTTP_REQUEST_GET,
   HTTP_REQUEST_POST,
   HTTP_RESPONSE,
};

namespace neweraHPC
{
   static struct _http_proxy
   {
      const char *host;
      const char *port;
   }*http_proxy = NULL;
#define http_proxy_host http_proxy->host
#define http_proxy_port http_proxy->port
   
   extern const char *request_type_strings[];
   
#define nhpc_http_request_is_post(h) (h->request_type == HTTP_RESPONSE_POST)
   struct http_data_t
   {
      char        *user_agent;
      char        *request_page;
      char	  *request_get;
      char	  *status_str;
      int	  request_type;
      char	  *content_type;
      int	  content_length;
      char	  *referer;
      char	  *origin;
      char	  *http_version;
      int          response_code;
      char        *location;
      char        *custom_response_data;
      short int    custom_response_type;
      char        *custom_response_mime;
      char	  *host;
      rbtree      *headers;
      nhpc_socket_t *sock;
   };
   
   extern rbtree *http_handlers;
   void http_init(network_t *network);
   void http_setup();

   nhpc_status_t http_handler_register(const char *handler_string, fnc_ptr_nhpc_t handler_function);
   
   void http_request_handler(nhpc_socket_t *sock);
   
   nhpc_status_t read_headers(rbtree *headers, http_data_t **http_data);
   nhpc_status_t delete_http_headers(http_data_t *http_data);
   
   void http_request(http_data_t *sock);
   void http_response(nhpc_socket_t *sock);
   
   nhpc_status_t http_get_file(http_data_t *http_data, const char **file_path, nhpc_socket_t **sock, const char *target_file, 
			       const char *host_addr, const char *host_port, const char *out_file = NULL);

   nhpc_status_t http_content_length(rbtree *headers, nhpc_size_t *size);
   nhpc_status_t http_response_code(rbtree *headers, int *response_code);
};

#endif