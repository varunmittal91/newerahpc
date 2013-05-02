/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
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

#ifndef _GRID_DATA_H_
#define _GRID_DATA_H_

#include <string.h>
#include <iostream>

#include <neweraHPC/rbtree.h>
#include <neweraHPC/network.h>

#include "grid_error.h"

using namespace std;

namespace neweraHPC
{
#define ARG_COUNT      6
#define ARG_RANGE      1
#define ARG_NUMBER     2
#define ARG_LITERAL    4
#define ARG_COMMAND    8
#define ARG_FILE      16
#define ARG_MEM_BLOCK 32
   static const char *grid_arg_content_types[] = {"ARG_RANGE", "ARG_NUMBER", "ARG_LITERAL", "ARG_COMMAND", "ARG_FILE", "ARG_MEM_BLCOK"};

   typedef unsigned char arg_t;
   static int _grid_arg_get_int_code(arg_t arg)
   {
      int code = 0;
      while((arg = (arg / 2)) > 0)
	 code++;
      return code;
   }
#define _grid_arg_is_type(a, t)   (1 & (a >> _grid_arg_get_int_code(t)))
#define _grid_arg_is_range(a)     (_grid_arg_is_type(a, ARG_RANGE))
#define _grid_arg_is_value(a)     (_grid_arg_is_type(a, ARG_VALUE))
#define _grid_arg_is_literal(a)   (_grid_arg_is_type(a, ARG_LITERAL))
#define _grid_arg_is_command(a)   (_grid_arg_is_type(a, ARG_COMMAND))
#define _grid_arg_is_file(a)      (_grid_arg_is_type(a, ARG_FILE))
#define _grid_arg_set_type(a, t)  ((*a) = t)
   
   static void grid_arg_set_value(const char **arg_value, const char *s)
   {
      (*arg_value) = new char [strlen(s) + sizeof(arg_t) + 1];
      nhpc_strcpy_noalloc((char *)((*arg_value) + sizeof(arg_t)), s);
   }
#define grid_arg_set_type(av, t)   (_grid_arg_set_type((char *)av, t))
   static void grid_arg_set_number(const char **arg_value, int number)
   {
      char *str = nhpc_itostr(number);
      grid_arg_set_value(arg_value, str);
      grid_arg_set_type(*arg_value, ARG_NUMBER);
      delete str;
   }
   static void grid_arg_set_range(const char **arg_value, int number1, int number2)
   {
      char *str1 = nhpc_itostr(number1);
      char *str2 = nhpc_itostr(number2);
      char *str3 = nhpc_strconcat(str1, ",", str2);
      grid_arg_set_value(arg_value, str3);
      grid_arg_set_type(*arg_value, ARG_RANGE);
      delete str1;
      delete str2;
      delete str3;
   }
   static void grid_arg_set_literals(const char **arg_value, const char *literal, arg_t arg)
   {
      grid_arg_set_value(arg_value, literal);
      grid_arg_set_type(*arg_value, arg);
   }
   static void grid_arg_set_address_space(const char **arg_value, void *address, nhpc_size_t size, arg_t arg)
   {
      nhpc_size_t len = sizeof(arg_t) + sizeof(nhpc_size_t) + sizeof(void *) + 1;
      
      char *_arg_value = new char [len];
      _arg_value[len]  = '\0';
      grid_arg_set_type(_arg_value, arg);
      *arg_value = _arg_value;
      
      char         **_address = (char **)((char *)(*arg_value) + sizeof(arg_t));
      nhpc_size_t   *_size    = (nhpc_size_t *)((char *)(*arg_value) + sizeof(arg_t) + sizeof(void *));
      *_address = (char *)address;
      *_size    = size;
   }
   static void *grid_arg_get_address_space(const char *arg_value, nhpc_size_t *size)
   {
      
   }
#define grid_arg_set_mem_block(av, a, s)  (grid_arg_set_address_space(av, a, s, ARG_MEM_BLOCK))
#define grid_arg_set_file(av, a, s)       (grid_arg_set_address_space(av, a, s, ARG_FILE))
#define grid_arg_set_literal(av, l)       (grid_arg_set_literals(av, l, ARG_LITERAL))
#define grid_arg_set_command(av, l)       (grid_arg_set_literals(av, l, ARG_COMMAND))
#define grid_arg_is_type(av, t)           (1 & (av >> _grid_arg_get_int_code(t)))
#define grid_arg_is_range(av)             (grid_arg_is_type(av, ARG_RANGE))
#define grid_arg_is_value(av)             (grid_arg_is_type(av, ARG_VALUE))
#define grid_arg_is_literal(av)           (grid_arg_is_type(av, ARG_LITERAL))
#define grid_arg_is_command(av)           (grid_arg_is_type(av, ARG_COMMAND))
#define grid_arg_is_file(av)              (grid_arg_is_type(av, ARG_FILE))
#define grid_arg_is_mem_block(av)         (grid_arg_is_type(av, ARG_MEM_BLOCK))
   

   struct grid_shared_data_t;
   typedef unsigned char status_t;
   struct grid_data_t
   {
      status_t       status;
      rbtree        *arguments;
      const char    *grid_uid;
      const char    *referer_grid_uid;
      const char    *peer_addr;
      const char    *peer_port;
      nhpc_socket_t *socket;

      grid_shared_data_t *data;
   };

#define grid_data_get_arguments(g)         (g->arguments) 
#define grid_data_get_argument_value(g, i) ((const char *)(*(grid_data_get_arguments(g)))[i])
#define grid_data_get_status(g)            (g->status)
#define grid_data_get_peer_addr(g)         (g->peer_addr)
#define grid_data_get_peer_port(g)         (g->peer_port)
#define grid_data_set_socket(g, s)         (g->socket = s)
#define grid_data_get_socket(g)            (g->socket)

#define grid_data_set_content_length(g, l) (g->content_len = l)
#define grid_data_set_content_addr(g, a)   (g->content_addr = a)
#define grid_data_set_content_type(g, t)   (nhpc_strcpy((char **)&(g->content_type), t))
   static void grid_data_set_opt(grid_data_t *data, int opt, bool on)
   {
      if(on)
	 data->status |= opt;
      else 
	 data->status &= ~opt;
   }
   static int grid_data_get_argument_count(grid_data_t *data)
   {
      if(data->arguments)
	 return data->arguments->length();
      else 
	 return 0;
   }
   static void grid_data_init(grid_data_t **data)
   {
      (*data) = new grid_data_t;
      memset((*data), 0, sizeof(grid_data_t));
   }
   static void grid_shared_data_destruct(grid_shared_data_t *data);
   static void grid_data_destruct(grid_data_t *data)
   {
      if(data->arguments)
      {
	 const char *value;
	 int count = grid_data_get_argument_count(data);
	 for(int i = 1; i <= count; i++)
	 {
	    value = grid_data_get_argument_value(data, i);
	    delete value;
	 }
	 
	 delete data->arguments;
      }
      
      if(data->data)
	 grid_shared_data_destruct(data->data);
   }   
   
   void grid_data_create_from_socket(grid_data_t *data, nhpc_socket_t *socket);
   
   struct grid_shared_data_t
   {
      void         *address;
      const char   *content_type;
      arg_t         arg;
      nhpc_size_t   len;
   };
#define grid_shared_data_get_data_address(g)  (g->address)
#define grid_shared_data_get_data_length(g)    (g->len)
   static void grid_shared_data_init(grid_shared_data_t **data)
   {
      (*data) = new grid_shared_data_t;
      memset((*data), 0, sizeof(grid_shared_data_t));
   }
   static void grid_shared_data_destruct(grid_shared_data_t *data)
   {
      delete data;
   }
   static void grid_shared_data_set_data(grid_shared_data_t *data, void *address, nhpc_size_t *len, arg_t arg)
   {
      int code = _grid_arg_get_int_code(arg);

      data->address      = address;
      data->len          = *len;
      data->arg          = arg;
      data->content_type = grid_arg_content_types[code];
   }
   static void grid_shared_data_get_headers(grid_shared_data_t *data, nhpc_headers_t *src_headers)
   {
      char *content_len_str = nhpc_longitostr(data->len);
      
      src_headers->insert("Content-Length", content_len_str);
      src_headers->insert("Content-Type"  , data->content_type);

      delete[] content_len_str;
   }
   static nhpc_status_t grid_shared_data_push_data(grid_shared_data_t *data, nhpc_socket_t *socket)
   {
      nhpc_size_t   size = data->len;
      nhpc_status_t nrv;
      
      nrv = socket_sendmsg(socket, (char *)(data->address), &size);
      return nrv;
   } 
   nhpc_status_t grid_shared_data_get_data(grid_shared_data_t **data, nhpc_socket_t *socket);
};

#endif
