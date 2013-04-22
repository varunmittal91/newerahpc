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

#ifndef _GRID_SERVER_H_
#define _GRID_SERVER_H_

#include <neweraHPC/constants.h>

namespace neweraHPC
{
   static network_t        *_network;
   static thread_manager_t *_thread_manager;
   
   static const char *_host_addr;
   static const char *_host_port;
   static int _host_core_count;
   static int _host_cpu_time;
#define grid_get_host_addr()        (_host_addr)
#define grid_get_host_port()        (_host_port)
#define grid_get_host_core_count()  (_host_core_count)
#define grid_get_host_cpu_time()    (_host_cpu_time)
   
   nhpc_status_t grid_server_init(int argc, char **argv);
   
   nhpc_status_t grid_register_to_controller(const char *grid_controller_addr, const char *grid_controller_port,
					     const char *host_addr, const char *host_port, int host_core_count,
					     int host_cpu_time);  
   nhpc_status_t grid_register_to_server(const char **grid_uid, const char *host_addr, const char *host_port);
};

#endif