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

#include <iostream>

#include <neweraHPC/error.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/grid.h>
#include <include/grid_request_handlers.h>
#include <include/grid_client_registration.h>
#include <include/grid_response.h>

using namespace std;

namespace neweraHPC 
{
   nhpc_status_t grid_communication_handlers_init()
   {
      grid_communication_handlers = new rbtree(RBTREE_STR);
      grid_communication_register_handler(grid_client_registration_handler, GRID_CLIENT_REGISTRATION);
      grid_communication_register_handler(grid_node_registration_handler, GRID_NODE_REGISTRATION);
      grid_communication_register_handler(grid_plugin_request_handler, GRID_PLUGIN_REQUEST);
      grid_communication_register_handler(grid_file_exchange_request_handler, GRID_FILE_EXCHANGE);
      grid_communication_register_handler(grid_instruction_request_handler, GRID_INSTRUCTION);
   }
   
   nhpc_status_t grid_communication_send(grid_communication_t *grid_communication)
   {
      nhpc_status_t nrv;
      const char *grid_uid = NULL;
      
      if(grid_communication_is_opt_register(grid_communication))
      {
	 nrv = grid_client_register_to_server(&grid_uid, grid_communication->dest_addr, grid_communication->dest_port);
	 if(nrv == NHPC_FAIL)
	 {
	    return nrv;
	 }
      }
      
      if(grid_uid)
      {
	 grid_communication->headers->insert("Grid-Uid", grid_uid);
	 grid_communication->grid_uid = grid_uid;
      }
      if(grid_communication_is_opt_send_peer_details(grid_communication))
      {
	 grid_communication->headers->insert("Peer-Host", _host_addr);
	 grid_communication->headers->insert("Peer-Port", _host_port);      
      }
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t grid_communication_push(grid_communication_t *grid_communication, grid_shared_data_t *_data)
   {
      const char *dest_addr = grid_communication->dest_addr;
      const char *dest_port = grid_communication->dest_port;

      nhpc_status_t       nrv;
      nhpc_socket_t      *socket;
      nhpc_headers_t     *headers = grid_communication->headers;
      
      grid_shared_data_t *data    = _data;
      if(!data)
	 data = grid_communication->data;

      nrv = socket_connect(&socket, dest_addr, dest_port, AF_INET, SOCK_STREAM, 0);
      grid_communication->socket = socket;

      if(data && nrv == NHPC_SUCCESS)
      {
	 grid_shared_data_get_headers(data, headers);
	 nrv = headers->write(socket);
	 nrv = grid_shared_data_push_data(data, socket);	 
      }
      else 
	 nrv = headers->write(socket);

      return nrv;
   }
   
   nhpc_status_t grid_request_handler(nhpc_socket_t *socket)
   {
      char       *command         = network_headers_get_param(socket->headers, "command");
      const char *uid             = network_headers_get_param(socket->headers, "Grid-Uid");
      const char *content_len_str = network_headers_get_param(socket->headers, "Content-Length");
      string_t   *string          = nhpc_substr(command, ' ');  
      
      if(string->count < 3)
      {
	 nhpc_string_delete(string);
	 return NHPC_FAIL;
      }

      nhpc_status_t  nrv     = NHPC_FAIL;
      const char    *fnc_str = string->strings[1];
      
      fnc_ptr_nhpc_t fnc_ptr = (fnc_ptr_nhpc_t)grid_communication_handlers->search(fnc_str);      
      
      if(fnc_ptr)
      {
	 grid_data_t *grid_data;
	 grid_data_init(&grid_data);
	 grid_data_create_from_socket(grid_data, socket);
	 
	 nrv = fnc_ptr(grid_data);
	 
	 grid_response_t *response;
	 grid_response_init(&response);
	 grid_response_set_socket(response, grid_data_get_socket(grid_data));
	 grid_shared_data_t *data = NULL;
	 if(nrv == NHPC_SUCCESS)
	 {
	    grid_set_response_status_code(response, GRID_RESPONSE_SUCCESSFUL);
	    if(grid_data->result_data)
	       data = grid_data->result_data;
	 }
	 else 
	    grid_set_response_status_code(response, GRID_RESPONSE_RESOURCE_UNAVAILABLE);
	 
	 grid_response_send(response);
	 grid_response_push(response, data);
	 grid_response_destruct(response);
	 grid_data_destruct(grid_data);
      }      
      else 
      {
	 LOG_ERROR("No Suitable GRID Handler found for the request");
      }
      
      nhpc_string_delete(string);

      return nrv;
   }
}