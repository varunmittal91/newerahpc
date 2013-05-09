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

#include <math.h>
#include <iostream>

#include <neweraHPC/rbtree.h>
#include <neweraHPC/network.h>
#include <neweraHPC/error.h>

#include <include/grid_data.h>
#include <include/grid_data_download.h>

using namespace std;

namespace neweraHPC
{
   void grid_data_create_from_socket(grid_data_t *data, nhpc_socket_t *socket)
   {
      rbtree *headers = socket->headers;
      const char *peer_addr = nhpc_socket_get_host_addr(socket);
      const char *peer_port = nhpc_socket_get_host_port(socket);
      nhpc_strcpy((char **)&(data->peer_addr), peer_addr);
      nhpc_strcpy((char **)&(data->peer_port), peer_port);
      
      grid_data_set_socket(data, socket);
      if(grid_shared_data_check(socket) == NHPC_SUCCESS)
	 grid_shared_data_get_data(&(data->input_data), socket);
   }
   
   nhpc_status_t grid_shared_data_check(nhpc_socket_t *socket)
   {
      rbtree        *headers            = socket->headers;
      const char    *check_content_len  = (const char *)socket->headers->search("Content-Length");
      const char    *check_content_type = (const char *)socket->headers->search("Content-Type");
      
      if(!check_content_len || !check_content_type)
	 return NHPC_FAIL;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t grid_shared_data_get_data(grid_shared_data_t **data, nhpc_socket_t *socket)
   {
      nhpc_status_t  nrv = NHPC_FAIL;
      rbtree        *headers            = socket->headers;
      const char    *check_content_len  = (const char *)socket->headers->search("Content-Length");
      const char    *check_content_type = (const char *)socket->headers->search("Content-Type");

      int code = -1;
      for(int i = 0; i < ARG_COUNT; i++)
      {
	 if(nhpc_strcmp(check_content_type, grid_arg_content_types[i]) == NHPC_SUCCESS)
	 {
	    code = i;
	    break;
	 }
      }
      if(code == -1)
      {
	 LOG_ERROR("GRID Data type not recognized");
	 return NHPC_FAIL;
      }
      
      arg_t        arg;
      void        *src_data;
      nhpc_size_t  content_len = nhpc_strtoi(check_content_len);

      arg = pow(2, code);

      nhpc_status_t (*fnc_ptr)(void **dst, nhpc_socket_t *socket, nhpc_size_t *content_len) = NULL;
      
      if(grid_arg_is_mem_block(arg))
      {
	 fnc_ptr = grid_data_download_memory_block;
      }
      else if(grid_arg_is_file(arg))
      {
	 fnc_ptr = grid_data_download_file;
      }
      
      if(fnc_ptr)
      {
	 nrv = fnc_ptr(&src_data, socket, &content_len);
	 grid_shared_data_init(data);
	 grid_shared_data_set_data(*data, src_data, &content_len, arg);
      }
      
      return nrv;
   }   
   
   nhpc_status_t grid_shared_data_push_data(grid_shared_data_t *data, nhpc_socket_t *socket)
   {
      nhpc_size_t   size = data->len;
      nhpc_status_t nrv;
      
      if(grid_arg_is_mem_block(data->arg))
      {
	 nrv = socket_sendmsg(socket, (char *)(data->address), &size);
	 return nrv;
      }
      else if(grid_arg_is_file(data->arg))
      {
	 nrv = grid_shared_data_push_file((const char *)(data->address), socket);
	 return nrv;
      }
      
      return NHPC_FAIL;
   }   
   
   nhpc_status_t grid_shared_data_push_file(const char *src_path, nhpc_socket_t *socket)
   {
      FILE *fp = fopen(src_path, "r");
      nhpc_size_t   len;
      nhpc_status_t nrv;

      char        buffer[10000];
      do 
      {
	 len = fread(buffer, 1, sizeof(buffer), fp);
	 nrv = socket_sendmsg(socket, buffer, &len);
      }while(nrv != EPIPE && !feof(fp) && nrv != NHPC_FAIL);
      
      fclose(fp);
      
      return nrv;
   }
}