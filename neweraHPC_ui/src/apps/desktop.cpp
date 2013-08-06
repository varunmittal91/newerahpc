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

#include <iostream>

#include <neweraHPC/network_data.h>
#include <neweraHPC/strings.h>

#include <include/app_triggers.h>
#include <include/webui_data.h>

using namespace std;
using namespace neweraHPC;

nhpc_status_t fnc_init_desktop(app_details_t *app_details)
{
   web_ui_set_app_name(app_details, "Desktop");
   web_ui_set_app_type(app_details, NUI_DESKTOP);
   web_ui_set_options(app_details, NUI_APP_OPT_DISABLE_MULTIPLE | NUI_APP_OPT_SYSTEM_UTILITY);
   web_ui_set_fnc_start(app_details, fnc_start_desktop);
   web_ui_set_fnc_processor(app_details, fnc_processor_desktop);
   
   return NHPC_SUCCESS;
}

nhpc_status_t fnc_start_desktop(nhpc_socket_t *socket, app_details_t *app_details)
{
   cout << "Starting desktop application" << endl;
}

nhpc_status_t fnc_processor_desktop(nhpc_socket_t *socket, app_details_t *app_details)
{
   
}

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
	 
	 //cout << (*appJson)["app_attributes"] << endl;
	 const char *app_type = (*appJson)["apptype"];
	 //cout << app_details->app_name << ":" << app_type << endl;
	 
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



