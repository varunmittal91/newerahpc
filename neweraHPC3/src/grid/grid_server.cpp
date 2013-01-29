/*
*
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

#include <iostream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/grid.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC 
{
   nhpc_status_t nhpc_register_to_controller(const char *grid_controller_addr, const char *grid_controller_port,
					     const char *host_addr, const char *host_port, int host_core_count,
					     int host_cpu_time)
   {
      nhpc_status_t nrv;
      nhpc_socket_t *sock;
      
      nrv = socket_connect(&sock, grid_controller_addr, grid_controller_port, AF_INET, SOCK_STREAM, 0);
      
      if(nrv != NHPC_SUCCESS)
      {
	 return errno;
      }      
      
      char *node_cores_str = nhpc_itostr(host_core_count);
      char *node_cpu_time_str = nhpc_itostr(host_cpu_time);
      
      nhpc_headers_t *headers = new nhpc_headers_t;
      headers->insert("GRID NODE_REGISTRATION 2.90");
      headers->insert("Node-Addr", host_addr);
      headers->insert("Node-Port", host_port);
      headers->insert("Node-Cores", node_cores_str);
      headers->insert("Node-Cpu-Time", node_cpu_time_str);
      headers->write(sock);
      
      nhpc_string_delete(node_cores_str);
      nhpc_string_delete(node_cpu_time_str);
      delete headers;
      
      socket_close(sock);
      socket_delete(sock);
   
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_client_registration_handler(nhpc_grid_server_t *grid_server,
								      nhpc_socket_t *socket)
   {
      nhpc_status_t nrv = (*grid_server).grid_client_registration(socket);
      
      return nrv;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_node_registration_handler(nhpc_grid_server_t *grid_server,
								    nhpc_socket_t *socket)
   {
      nhpc_status_t nrv = (*grid_server).grid_node_registration(socket);
      
      return nrv;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_plugin_request_handler(nhpc_grid_server_t *grid_server,
								 nhpc_socket_t *socket)
   {
      char       *plugin    = network_headers_get_param(socket->headers, "Plugin");
      const char *peer_host = network_headers_get_param(socket->headers, "Peer-Host");
      const char *peer_port = network_headers_get_param(socket->headers, "Peer-Port");
      const char *uid       = network_headers_get_param(socket->headers, "Grid-Uid");

      nhpc_status_t nrv = NHPC_FAIL;
      
      plugin_details_t *plugin_details;
      grid_server->search_plugin(plugin, &plugin_details);
      
      if(!plugin_details)
	 nrv = NHPC_FAIL;
      else 
      {
	 const char *src_path;
	 
	 if(plugin_details->path_nxi)
	    src_path = plugin_details->path_nxi;
	 else 
	    src_path = plugin_details->path_plugin;
	 
	 nrv = nhpc_send_file(uid, peer_host, peer_port, src_path);
      }      
      
      return nrv;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_instruction_request_handler(nhpc_grid_server_t *grid_server,
								  nhpc_socket_t *socket)
   {
      nhpc_status_t  nrv;
      const char    *uid = network_headers_get_param(socket->headers, "Grid-Uid");
      
      nhpc_instruction_set_t *instruction_set;
      
      LOG_DEBUG("My Max Cpu time: " << grid_server->host_cpu_time);
      
      nrv = nhpc_generate_instruction(&instruction_set, (rbtree *)socket->headers);
      nrv = grid_server->grid_execute(instruction_set, socket, &uid);      
      
      return nrv;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_submission_request_handler(nhpc_grid_server_t *grid_server,
								     nhpc_socket_t *socket)
   {
      char *peer_id   = network_headers_get_param(socket->headers, "Peer");
      int   peer_id_n = nhpc_strtoi(peer_id);
      
      if(socket->partial_content)
      {
	 task_t *task = (task_t *)socket->partial_content;
	 nhpc_systeminfo_t *systeminfo = &(task->systeminfo);
	 
	 LOG_DEBUG("Node:load_avg_max: " << systeminfo->cpuinfo.load_avg_max);
	 LOG_DEBUG("Node:meminfo: " << systeminfo->meminfo.free_mem);
	 
	 grid_server->free_peer(nhpc_strtoi(peer_id), systeminfo->cpuinfo.load_avg_max, &(systeminfo->meminfo));
      }
      else 
	 grid_server->free_peer(nhpc_strtoi(peer_id));
      
      return NHPC_SUCCESS;      
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_file_exchange_request_handler(nhpc_grid_server_t *grid_server,
									nhpc_socket_t *socket)
   {
      LOG_INFO("Downloading file");
      
      char       *file_name     = network_headers_get_param(socket->headers, "File-Name");
      char       *file_type     = network_headers_get_param(socket->headers, "File-Type");
      char       *file_size_str = network_headers_get_param(socket->headers, "Content-Length");
      const char *uid           = network_headers_get_param(socket->headers, "Grid-Uid");
      
      if(!file_name || !file_type || !file_size_str)
      {
	 LOG_ERROR("Error downloading file");
	 return NHPC_FAIL;
      }
      
      nhpc_size_t file_size = nhpc_strtoi(file_size_str);
      
      nhpc_size_t size_downloaded = 0;
      char buffer[10000];
      nhpc_status_t nrv;
      nhpc_size_t size;
      
      const char *final_path = nhpc_strconcat("/www/grid/", uid, "/", file_name);
      
      int fd = -1;
      int retry_count = 0;
      do 
      {
	 fd = open (final_path, O_WRONLY | O_CREAT | O_NONBLOCK | O_NOCTTY,
		    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	 retry_count++;
      }while(fd == -1 && retry_count < 5);
      
      if(fd == -1 && close(fd) < 0)
      {
	 nhpc_string_delete((char *)final_path);
	 return NHPC_FAIL;
      }
      
      FILE *fp = fopen(final_path, "wb");
      
      if(socket->partial_content != NULL)
      {
	 fwrite(socket->partial_content, 1, socket->partial_content_len, fp);
	 size_downloaded += socket->partial_content_len;
      }
      
      do 
      {
	 bzero(buffer, sizeof(buffer));
	 size = sizeof(buffer);
	 nrv = socket_recv(socket, buffer, &size); 
	 fwrite(buffer, 1, size, fp);	 
	 size_downloaded += size;
      }while((nrv != NHPC_EOF) && file_size != size_downloaded);
      
      fclose(fp);
      
      if(nhpc_strcmp(file_type, "plugin") == NHPC_SUCCESS)
      {
	 nrv = (*grid_server).install_plugin(final_path, uid);
	 if(nrv != NHPC_SUCCESS)
	 {
	    nhpc_string_delete((char *)final_path);
	    return NHPC_FAIL;
	 }
      }
      
      nhpc_string_delete((char *)final_path);
      
      return NHPC_SUCCESS;
   }   
};


