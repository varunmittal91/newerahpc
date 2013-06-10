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

#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/error.h>
#include <include/strings.h>
#include <include/file.h>
#include <include/http.h>
#include <include/headers.h>
#include <include/sockets.h>
#include <include/network.h>
#include <include/neweraHPC.h>
#include <include/http_data.h>

namespace neweraHPC
{
   rbtree       *http_handlers;
   nhpc_mutex_t  mutex_http_handlers;
   
   nhpc_status_t http_init()
   {
      nhpc_status_t nrv;
      
      if((nrv = nhpc_network_register_addon("*HTTP/1.1", http_request_handler)) != NHPC_SUCCESS)
	 return nrv;
      
      http_setup();
      
      return NHPC_SUCCESS;      
   }
   
   void http_setup()
   {
      http_handlers = new rbtree(RBTREE_STR);
      thread_mutex_init(&mutex_http_handlers);
      
      const char *test_proxy = nhpc_get_cmdline_argument("http-proxy");  
      
      if(test_proxy)
      {
	 int pos = nhpc_strfind(test_proxy, ':');
	 if(pos)
	 {
	    const char *host = nhpc_substr(test_proxy, 1, pos - 1);
	    const char *port = nhpc_substr(test_proxy, pos + 1, strlen(test_proxy));
	    http_proxy = new _http_proxy;
	    memset(http_proxy, 0, sizeof(_http_proxy));
	    http_proxy->host = host;
	    http_proxy->port = port;
	 }
      }      
   }
   
   void http_request_handler(nhpc_socket_t *socket)
   {
      nhpc_status_t  nrv;
      http_data_t   *http_data;
      if((nrv = http_data_prepare(&http_data, socket)) != NHPC_SUCCESS)
	 return;

      http_request(http_data);
      http_data_destruct(http_data);
   }

   void http_request(http_data_t *http_data)
   {
      const char *request     = http_data->request;
      const char *app_name    = NULL;
      string_t   *request_str = nhpc_substr(request, '/');
    
      http_handler_ptr_t http_handler = NULL;      
      if(request_str)
      {
	 app_name = request_str->strings[0];
	 if(!(http_handler = http_handler_search_app(app_name)))
	 {
	    app_name = NULL;
	 }
	 
	 nhpc_string_delete(request_str);
      }

      if(http_handler)
	 http_handler(http_data);

      if(!http_data_check_opt(http_data, HTTP_OPT_DATA_SET))
      {
	 http_request_check(http_data);
      }
      
      http_content_t *content;
      if(http_data_check_opt(http_data, HTTP_OPT_NEW_ALLOCATED))
      {
	 content = ((*http_data).content.new_allocated);
      }
      else 
	 content = ((*http_data).content.pre_allocated);
      
      http_request_send(http_data, content);
   }
   
   void http_request_check(http_data_t *http_data)
   {
      const char    *request     = NULL;
      string_t      *request_str = nhpc_substr(http_data->request, '/');

      if(request_str)
      {
	 nhpc_strcpy((char **)&request, HTTP_ROOT);
	 const char *tmp_request = NULL;
	 
	 for(int i = 0; i < request_str->count; i++)
	 {
	    if(nhpc_strcmp("..", request_str->strings[i]) == NHPC_SUCCESS ||
	       nhpc_strcmp("." , request_str->strings[i]) == NHPC_SUCCESS)
	       continue;
	    
	    tmp_request = nhpc_strconcat(request, "/", request_str->strings[i]);
	    delete[] request;
	    request = tmp_request;
	 }

	 nhpc_string_delete(request_str);
      }
      else 
	 request = nhpc_strconcat(HTTP_ROOT, "/");
      
      http_data_add_content(http_data, (void *)request, NULL, 0, HTTP_CONTENT_TYPE_DIR, HTTP_OPT_NEW_ALLOCATED);
      
      delete[] request;
   }
   
   void http_request_send(http_data_t *http_data, http_content_t *content)
   {
      int          response_code     = http_status_get_code(http_data->status);
      const char  *response_code_str = nhpc_itostr(response_code);
      const char  *response_str      = http_status_get_string(http_data->status);
      const char  *header_str        = nhpc_strconcat("HTTP/1.1 ", response_code_str, " ", response_str);
      const char  *content_len_str   = NULL;
      const char  *content_mime;
      nhpc_size_t  content_len;
      const char  *content_data;
      
      if((content_len = (*content).len) > 0)
	 content_len_str = nhpc_itostr((*content).len);
      content_mime    = (*content).content_mime;
      content_data    = (const char *)(*content).content_data;
      
      nhpc_headers_t *headers = new nhpc_headers_t;
      (*headers).insert(header_str);
      if(content_len_str)
	 (*headers).insert("Content-Length", content_len_str);
      if(content_mime)
	 (*headers).insert("Content-Type", content_mime);
      
      (*headers).write(http_data->socket);
      
      delete   headers;
      delete[] header_str;
      delete[] response_code_str;
      if(content_len_str)
	 delete[] content_len_str;
      
      if(http_content_is_mem(content))
	 socket_sendmsg((*http_data).socket, content_data, &content_len);
      else if(http_content_is_file(content))
	 socket_sendfile(http_data->socket, content_data);
      else if(http_content_is_dir(content))
	 http_request_send_dir(http_data, content);
   }
	      
   void http_request_send_dir(http_data_t *http_data, http_content_t *content)
   {
      const char *data = "<html><head><title>Directory Information</title></head><body><h1>Directory List</h1><table>";
      nhpc_size_t size = strlen(data);      
      socket_sendmsg(http_data->socket, data, &size);
      
      string_t *dirlist_str = nhpc_get_file_list((const char *)(content->content_data), NHPC_VISIBLE_DIR_CHILD);
      for(int i = 0; i < dirlist_str->count; i++)
      {
	 data = "<tr><td>";
	 size = strlen(data);      
	 socket_sendmsg(http_data->socket, data, &size);
	 
	 data = (const char *)(dirlist_str->strings[i]);
	 size = strlen(data);      
	 socket_sendmsg(http_data->socket, data, &size);
	 
	 data = "</td></tr>";
	 size = strlen(data);      
	 socket_sendmsg(http_data->socket, data, &size);
      }
      
      data = "</table></body></html>";
      size = strlen(data);      
      socket_sendmsg(http_data->socket, data, &size);
      
      nhpc_string_delete(dirlist_str);
   }

   http_handler_ptr_t http_handler_search_app(const char *app_name)
   {
      http_handlers_lock(NHPC_THREAD_LOCK_READ);
      http_handler_ptr_t fnc_ptr = (http_handler_ptr_t)(*http_handlers).search(app_name);
      http_handlers_unlock(NHPC_THREAD_LOCK_READ);
      
      return fnc_ptr;
   }
   
   nhpc_status_t http_handler_register_app(const char *app_name, http_handler_ptr_t fnc_ptr)
   {
      nhpc_status_t nrv = NHPC_SUCCESS;
      
      http_handlers_lock(NHPC_THREAD_LOCK_WRITE);
      if(!((*http_handlers).insert((void *)fnc_ptr, app_name)))
	 nrv = NHPC_FAIL;
      http_handlers_unlock(NHPC_THREAD_LOCK_WRITE);
      
      return nrv;
   }
};
