/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
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

#include <iostream>

#include <include/grid.h>
#include <include/network.h>
#include <include/file.h>

using namespace std;

namespace neweraHPC
{
   nhpc_status_t nhpc_register_to_server(const char **grid_uid, const char *host_addr, const char *host_port)
   {
      nhpc_socket_t *sock;
      
      nhpc_status_t nrv = socket_connect(&sock, host_addr, host_port, AF_INET, SOCK_STREAM, 0);
      cout<<sock->sockfd<<endl;
      
      if(nrv != NHPC_SUCCESS)
      {
	 socket_delete(sock);
	 
	 return NHPC_FAIL;
      }
      
      const char *mssg = "GRID CLIENT_REGISTRATION 2.90\r\n\r\n";
      size_t size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == NHPC_FAIL)
      {
	 socket_close(sock);
	 socket_delete(sock);
	 
	 return NHPC_FAIL;
      }
      
      size = 1000;
      char buffer[1000];
      
      nrv = 0;
      
      nrv = socket_recv(sock, buffer, &size);
      if(nrv == NHPC_SUCCESS)
      {
	 nhpc_strcpy((char **)grid_uid, buffer);
      }
      else 
	 nrv = NHPC_FAIL;
      
      socket_close(sock);
      socket_delete(sock);
      
      return nrv;
   }
   
   nhpc_status_t nhpc_send_plugin(const char *grid_uid, const char *host_addr, 
				  const char *host_port, const char *plugin_path)
   {
      nhpc_socket_t *sock;
      
      nhpc_status_t nrv = socket_connect(&sock, host_addr, host_port, AF_INET, SOCK_STREAM, 0);
      cout<<sock->sockfd<<endl;
      nhpc_size_t size;
      
      if(nrv != NHPC_SUCCESS)
      {
	 socket_delete(sock);
	 
	 return NHPC_FAIL;
      }      
      
      string_t *string = nhpc_substr(plugin_path, '/');
      nhpc_size_t file_len;
      nhpc_file_size(plugin_path, &file_len);
      
      header_t headers[5];
      
      char *arg_grid_uid = nhpc_strconcat("Grid-Uid: ", grid_uid);
      char *arg_file_name = nhpc_strconcat("File-Name: ", string->strings[string->count - 1]);
      char *arg_content_len = nhpc_strconcat("Content-Length: ", nhpc_itostr(file_len));
      
      nhpc_strcpy(&(headers[0].string), "GRID FILE_EXCHANGE 2.90\r\n");
      headers[1].string = nhpc_strconcat(arg_grid_uid, "\r\n");
      nhpc_strcpy(&(headers[2].string), "File-Type: plugin\r\n");
      headers[3].string = nhpc_strconcat(arg_file_name, "\r\n");
      headers[4].string = nhpc_strconcat(arg_content_len, "\r\n\r\n");
      
      delete[] arg_grid_uid;
      delete[] arg_file_name;
      delete[] arg_content_len;
      
      for(int i = 0; i < 5; i++)
      {
	 size = strlen(headers[i].string);
	 nrv = socket_send(sock, headers[i].string, &size);
	 delete[] (headers[i].string);
	 if(nrv != NHPC_SUCCESS)
	    return nrv;
      }
      
      FILE *fp = fopen(plugin_path, "r");
      nhpc_size_t len;
      char buffer[1000];
      
      do
      {
	 len = fread(buffer, 1, sizeof(buffer), fp);
	 nrv = socket_send(sock, buffer, &len);	
      }while(nrv != EPIPE && len != 0);      
            
      fclose(fp);
      
      bzero(buffer, 1000);
      len = 5;
      do 
      {
	 nrv = socket_recv(sock, buffer, &len);
      }while(nrv != NHPC_SUCCESS && nrv != NHPC_EOF);
      nrv = nhpc_strtoi(buffer);
      cout<<nrv<<endl;
      
      socket_close(sock);
      socket_delete(sock);
      
      return NHPC_SUCCESS;
   }
};
