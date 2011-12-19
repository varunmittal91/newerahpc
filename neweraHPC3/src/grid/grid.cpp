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
#include <include/thread.h>

using namespace std;

namespace neweraHPC
{
   class thread_manager_t;
   
   void grid_init(nhpc_socket_t *sock)
   {
      cout<<"GRID Request Encountered"<<endl;
   }
   
   nhpc_grid_server_t::nhpc_grid_server_t(thread_manager_t **in_thread_manager) : plugin_manager_t(in_thread_manager)
   {
   }
   
   void nhpc_grid_server_t::grid_server_init()
   {
      plugin_manager_init();
   }
   
   nhpc_grid_server_t::~nhpc_grid_server_t()
   {
      
   }
   
   void nhpc_grid_server_t::grid_request_init(nhpc_socket_t *sock)
   {
      cout<<"GRID Request Encountered"<<endl;
   }
};