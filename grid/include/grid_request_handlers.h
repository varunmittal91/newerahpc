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
 * 	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GRID_REQUEST_HANDLER_H_
#define _GRID_REQUEST_HANDLER_H_

namespace neweraHPC
{
   nhpc_status_t grid_client_registration_handler(grid_data_t *grid_data);
   nhpc_status_t grid_node_registration_handler(grid_data_t *grid_data);
   nhpc_status_t grid_plugin_request_handler(grid_data_t *grid_data);
   nhpc_status_t grid_file_exchange_request_handler(grid_data_t *grid_data);
   nhpc_status_t grid_instruction_request_handler(grid_data_t *grid_data);
   nhpc_status_t grid_submission_request_handler(grid_data_t *grid_data);   
}

#endif