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
#include "sockets.h"
#include "http_data.h"

#define HTTP_ROOT         "/www"
#define HTTP_DEFAULT_FILE "index.html"

namespace neweraHPC
{
   typedef void (*http_handler_ptr_t)(http_data_t *);
   extern rbtree       *http_handlers;
   extern nhpc_mutex_t  mutex_http_handlers;
#define http_handlers_lock(m)   (thread_mutex_lock(&mutex_http_handlers, m))
#define http_handlers_unlock(m) (thread_mutex_unlock(&mutex_http_handlers, m))
   
   static struct _http_proxy
   {
      const char *host;
      const char *port;
   }*http_proxy = NULL;
#define http_proxy_host http_proxy->host
#define http_proxy_port http_proxy->port
      
   nhpc_status_t http_init();   
   void          http_setup();
   
   void http_request_handler(nhpc_socket_t *socket);
   void http_request(http_data_t *http_data);
   void http_request_check(http_data_t *http_dat);
   void http_request_send(http_data_t *http_data, http_content_t *content);
   void http_request_send_dir(http_data_t *http_data, http_content_t *content);
   
   http_handler_ptr_t http_handler_search_app(const char *app_name);
   nhpc_status_t      http_handler_register_app(const char *app_name, http_handler_ptr_t fnc_ptr);
};

#endif