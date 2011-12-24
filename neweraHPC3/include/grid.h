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

#ifndef _GRID_H_
#define _GRID_H_

#include "network_data.h"
#include "grid_data.h"
#include "grid_scheduler.h"
#include "grid_plugin.h"
#include "grid_instruction_set.h"
#include "grid_client.h"

namespace neweraHPC 
{
   class nhpc_grid_server_t : public plugin_manager_t, public grid_scheduler_t
   {
   private:
      struct functions_t
      {
	 fnc_ptr_nhpc_two_t  client_registration;
	 fnc_ptr_nhpc_t      file_download;
      };
      
      rbtree_t *functions_rbtree;
      rbtree_t *clients;
      functions_t *functions;
      fnc_ptr_t ptr;
      nhpc_status_t grid_client_gen_uid(const char *client_addr, const char **uid);
      nhpc_status_t grid_client_verify_uid(const char *uid);
   public:
      nhpc_grid_server_t(thread_manager_t **in_thread_manager);
      ~nhpc_grid_server_t();
      void grid_server_init();
      void grid_request_init(nhpc_socket_t *sock);
      nhpc_status_t grid_file_download(nhpc_socket_t *sock, const char **grid_uid);
      nhpc_status_t grid_execute(nhpc_socket_t *sock, const char **grid_uid);
      
      nhpc_status_t grid_client_registration(nhpc_socket_t *sock);
   };
   
   void grid_init(nhpc_socket_t *sock);   
   
   nhpc_status_t nhpc_grid_file_download(nhpc_socket_t *sock, const char **file_path);
};

#endif