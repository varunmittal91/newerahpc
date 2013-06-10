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

#ifndef _HTTP_DATA_H_
#define _HTTP_DATA_H_

#include <string.h>
#include <iostream>

#include "rbtree.h"
#include "sockets.h"
#include "http_status.h"

using namespace std;

namespace neweraHPC
{
   inline int _get_bit_position(int bit)
   {
      int i = 0;
      while(bit > 1)
      {
	 bit /= 2;
	 i++;
      }
      
      return i;
   }
   
   struct http_content_t
   {
#define HTTP_CONTENT_TYPE_DIR  1
#define HTTP_CONTENT_TYPE_FILE 2
#define HTTP_CONTENT_TYPE_MEM  4
      typedef unsigned char content_type;
#define content_set_type(c, t)  (c = t)
#define content_is_type(c, t)   (1 & (c >> _get_bit_position(t)))
#define content_is_dir(c)       (content_is_type(c, HTTP_CONTENT_TYPE_DIR))
#define content_is_file(c)      (content_is_type(c, HTTP_CONTENT_TYPE_FILE))
#define content_is_mem(c)       (content_is_type(c, HTTP_CONTENT_TYPE_MEM))
      
      void *content_data;
      char *content_mime;
      nhpc_size_t  len;
      content_type type;
   };
#define http_content_get_type(r)     ((r)->type)
#define http_content_set_type(r, t)  (content_set_type(http_content_get_type(r), t))
#define http_content_is_type(r, t)   (content_is_type(http_content_get_type(r), t))
#define http_content_is_dir(r)       (http_content_is_type(r, HTTP_CONTENT_TYPE_DIR))
#define http_content_is_file(r)      (http_content_is_type(r, HTTP_CONTENT_TYPE_FILE))
#define http_content_is_mem(r)       (http_content_is_type(r, HTTP_CONTENT_TYPE_MEM))
   
   static void http_content_init(http_content_t **content)
   {
      (*content) = new http_content_t;
      memset((*content), 0, sizeof(http_content_t));
   }
   static void http_content_destruct(http_content_t *content)
   {
      if(content->content_mime)
	 delete[] (content->content_mime);
      
      delete content;
   }
   
   struct http_data_t
   {
      rbtree        *headers;
      nhpc_socket_t *socket;
      
      http_status_t status;
#define http_status_set_status(h, s)   (_http_status_set_status(h->status, s))
#define http_status_check_status(h, s) (_http_status_check_status(h->status, s))
      
#define HTTP_REQUEST_GET  1
#define HTTP_REQUEST_POST 2
#define HTTP_RESPONSE     4
      
#define HTTP_OPT_PRE_ALLOCATED  8
#define HTTP_OPT_NEW_ALLOCATED 16
#define HTTP_OPT_DATA_SET      32
      typedef unsigned char request_type_t;
      request_type_t type;
      
      const char  *host;                // taken from network headers
      const char  *user_agent;          // taken from network headers
      const char  *request;             // allocated
      const char  *location;            // taken from network headers
      rbtree      *get_variables;       // not imlemented yet
      nhpc_size_t  content_length;      
      const char  *content_type;        // taken from network headers
      union _content
      {
	 http_content_t *pre_allocated;
	 http_content_t *new_allocated;
      }content;
   };
   nhpc_status_t http_data_add_content(http_data_t *http_data, void *address, char *mime, nhpc_size_t len, 
				       int type, int opt = HTTP_OPT_NEW_ALLOCATED);
   
#define http_data_set_type(h, t)    ((h)->type = t)
#define http_request_is_invalid(h)  (http_check_request_type(h, HTTP_INVALID))
#define http_request_is_get(h)      (http_check_request_type(h, HTTP_REQUEST_GET))
#define http_request_is_post(h)	    (http_check_request_type(h, HTTP_REQUEST_POST))
#define http_request_is_response(h) (http_check_request_type(h, HTTP_RESPONSE))
   
#define http_data_check_opt(h, o)  (1 & ((h->type) >> _get_bit_position(o)))
#define http_data_set_opt(h, o)    ((h)->type |= o)
   
   void          http_data_init(http_data_t **http_data);
   void          http_data_destruct(http_data_t *http_data);
   nhpc_status_t http_data_prepare(http_data_t **http_data, nhpc_socket_t *socket);
};

#endif