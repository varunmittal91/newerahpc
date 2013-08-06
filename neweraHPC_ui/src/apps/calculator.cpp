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

#include <neweraHPC/network_data.h>

#include <include/nui.h>
#include <include/webui_data.h>

using namespace neweraHPC;

nhpc_status_t func_trigger_calculator(nhpc_socket_t *sock, web_ui_elements_t *web_ui_elements)
{
   app_details_t *app_details = (app_details_t *)(*app_handlers).search(web_ui_elements->app_name);
   int instance_id = app_details->instances->length();
   
   web_ui_elements->elements->add_element(JSON_OBJECT, "NuiWindow");
   web_ui_elements->elements->add_element(JSON_STRING, "title", "Calculator");
   web_ui_elements->elements->add_element(JSON_STRING, "height", "80%");
   web_ui_elements->elements->add_element(JSON_STRING, "width", "80%");
   web_ui_elements->elements->add_element(JSON_TRUE, "draggable");
   
   
   web_ui_elements->elements->add_element(JSON_OBJECT, "NuiTable");
   web_ui_elements->elements->add_element(JSON_STRING, "id", "button_panel");
   web_ui_elements->elements->add_element(JSON_TRUE, "visible");
   web_ui_elements->elements->add_element(JSON_NUMBER, "n_rows", "4");
   web_ui_elements->elements->add_element(JSON_NUMBER, "n_columns", "6");
   web_ui_elements->elements->add_element(JSON_ARRAY, "NuiElements");
   web_ui_elements->elements->add_element(JSON_OBJECT);
   web_ui_elements->elements->add_element(JSON_OBJECT, "NuiButton");
   web_ui_elements->elements->add_element(JSON_STRING, "id", "calc_4_button");   
   web_ui_elements->elements->add_element(JSON_STRING, "label", "4");
   web_ui_elements->elements->add_element(JSON_OBJECT, "mouse_event");
   web_ui_elements->elements->add_element(JSON_STRING, "event", "onmousedown");
   web_ui_elements->elements->add_element(JSON_STRING, "action", "javascript:buttonpress();");
   web_ui_elements->elements->close_element();
   web_ui_elements->elements->close_element();
   web_ui_elements->elements->close_element();
   web_ui_elements->elements->close_element();
   web_ui_elements->elements->close_element();
   
   web_ui_elements->elements->close_element();
   
   
   web_ui_elements->elements->close_element();
   
   web_ui_elements->add_public_data();
   
   return NHPC_SUCCESS;
}


