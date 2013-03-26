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

#include <include/grid_communication.h>
#include <include/grid_client_registration.h>

namespace neweraHPC
{
   nhpc_status_t grid_client_register_to_server(const char **grid_uid, const char *host_addr, const char *host_port)
   {
      nhpc_status_t nrv;
      
      grid_communication_t *grid_communication;
      grid_communication_init(&grid_communication, GRID_CLIENT_REGISTRATION);
      grid_set_communication_opt(grid_communication, GRID_COMMUNICATION_OPT_REGISTER);
      grid_communication_add_dest(grid_communication, host_addr, host_port);
      grid_communication_send(grid_communication);
      nrv = grid_communication_push(grid_communication);      
      
      
      
      grid_communication_destruct(grid_communication);

      return nrv;
   }
}