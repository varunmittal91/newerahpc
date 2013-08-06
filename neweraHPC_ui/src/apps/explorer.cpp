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

nhpc_status_t func_trigger_explorer(nhpc_socket_t *sock, web_ui_elements_t *web_ui_elements)
{
   json_t *json = web_ui_elements->elements;
   
   if(!(web_ui_elements->is_ready))
   {      
      json->close_element();
      
      web_ui_elements->is_ready = true;
   }
}

