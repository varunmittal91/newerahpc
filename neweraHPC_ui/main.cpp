/*
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

#include <neweraHPC/neweraHPC.h>

#include "include/app_triggers.h"

using namespace neweraHPC;

int main(int argc, char **argv)
{
   neweraHPC_init(argc, argv);
   
   http_init();
   web_ui_init();
   
   web_ui_register((fnc_ptr_nhpc_t)fnc_init_desktop);   
   //web_ui_register("Explorer",    "NuiApplication", (fnc_ptr_nhpc_two_t)func_trigger_explorer);
   //web_ui_register("Calculator",  "NuiApplication", (fnc_ptr_nhpc_two_t)func_trigger_calculator);   
   //web_ui_register("Dock",        "NuiUtility",     (fnc_ptr_nhpc_two_t)func_trigger_dock);
   //web_ui_register("System_info", "NuiApplication", (fnc_ptr_nhpc_two_t)func_trigger_system_status);
   
   nhpc_status_t nrv;
   
   nhpc_grid_server_t grid_server;
   nrv = grid_server.grid_server_init();
   
   if(nrv != NHPC_SUCCESS)
      LOG_ERROR("Grid initialization failed");
   else 
      grid_server.grid_server_join();
}
