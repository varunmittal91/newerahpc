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

#include <string.h>
#include <iostream>

#include <neweraHPC/sockets.h>

#include <include/grid_data_download.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t grid_data_download_memory_block(void **dst, nhpc_socket_t *socket, nhpc_size_t *content_len)
   {
      char *tmp_dst;
      char *dst_str = new char [*content_len];
      memset(dst_str, 0, *content_len);
      
      char          buffer[1000];
      nhpc_size_t   size;
      nhpc_size_t   size_downloaded = 0;
      nhpc_status_t nrv;

      if(socket->partial_content)
      {
	 memcpy(dst_str, (socket->partial_content), (socket->partial_content_len));
	 tmp_dst         += (socket->partial_content_len);
	 size_downloaded += (socket->partial_content_len);
      }
      else 
	 tmp_dst = dst_str;
      
      do 
      {
	 bzero(buffer, sizeof(buffer));
	 size = sizeof(buffer);
	 nrv = socket_recv(socket, buffer, &size); 
	 memcpy(tmp_dst, buffer, size);
	 
	 size_downloaded += size;
	 memcpy(tmp_dst, buffer, size);
	 tmp_dst += size;
      }while(nrv != NHPC_EOF && size_downloaded != *content_len);      
      
      if(nrv == NHPC_EOF)
	 nrv = NHPC_SUCCESS;
      
      *dst = dst_str;
      
      return nrv;
   }
};