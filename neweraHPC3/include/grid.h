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
   class nhpc_grid_server_t : public plugin_manager_t
   {
   private:
      struct functions_t
      {
	 fnc_ptr_t client_registration;
      };
      
      rbtree_t *functions_rbtree;
      functions_t *functions;
      fnc_ptr_t ptr;
      
   public:
      nhpc_grid_server_t(thread_manager_t **in_thread_manager);
      ~nhpc_grid_server_t();
      void grid_server_init();
      void grid_request_init(nhpc_socket_t *sock);
      
      static nhpc_status_t *grid_client_registration(nhpc_grid_server_t *grid_server);
   };
   
   void grid_init(nhpc_socket_t *sock);
   
   nhpc_status_t nhpc_register_to_server(const char *host_addr, const char *host_port);
};

#endif