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

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/rbtree.h>
#include <include/neweraHPC.h>

using namespace std;
using namespace neweraHPC;

nhpc_status_t func_trigger_desktop(nhpc_socket_t *sock, web_ui_elements_t *web_ui_elements)
{
   if(!(web_ui_elements->is_ready))
   {
      json_t *json = web_ui_elements->elements;
      
      json->add_element(JSON_STRING, "apptype", "NuiDesktop");
      
      json->add_element(JSON_ARRAY, "requires");
      json->add_element(JSON_STRING, "Dock");
      json->close_element();
      
      json->add_element(JSON_OBJECT, "NuiDesktop");      
      json->add_element(JSON_OBJECT, "icon_grid");
      json->add_element(JSON_ARRAY, "data");
      
      
      for(int i = 1; i <= app_handlers->length(); i++)
      {
	 app_details_t *app_details = (app_details_t *)(*app_handlers)[i];
	 json_t *appJson            = app_details->web_ui_elements->elements;
	 
	 cout << (*appJson)["app_attributes"] << endl;
	 const char *app_type = (*appJson)["apptype"];
	 cout << app_details->app_name << ":" << app_type << endl;
	 
	 web_ui_elements->elements->add_element(JSON_OBJECT);
	 web_ui_elements->elements->add_element(JSON_STRING, "id", (app_details->app_name));
	 web_ui_elements->elements->close_element();
      }
      json->close_element();
      json->close_element();
      json->close_element();
      
      json->close_element();
      
      json->print();
      
      web_ui_elements->is_ready = true;
   }
   
   return NHPC_SUCCESS;
}

int main(int argc, char **argv)
{   
   rbtree *test_tree1 = new rbtree;
      
   neweraHPC_init(argc, argv);

   nhpc_status_t nrv;   
   
   http_init();
   //web_ui_init();
   
   //web_ui_register("Desktop" , "NuiDesktop", (fnc_ptr_nhpc_two_t)func_trigger_desktop);   
   //web_ui_register("Explorer", "NuiWindow" , NULL);

   nhpc_grid_server_t grid_server;
   nrv = grid_server.grid_server_init();
   
   if(nrv != NHPC_SUCCESS)
      LOG_ERROR("Grid initialization failed");
   
   return 0;
}
