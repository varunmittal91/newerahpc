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

#ifndef _WEBUI_H_
#define _WEBUI_H_

#include "webui_data.h"

#include <neweraHPC/http.h>

namespace neweraHPC
{
   nhpc_status_t web_ui_init();
   
   nhpc_status_t web_ui_init_app(app_details_t *app_details);
   
   nhpc_status_t web_ui_handler(http_data_t *http_data);
   
   nhpc_status_t web_ui_check_app(app_details_t **app_details, http_data_t *http_data);
   
   nhpc_status_t web_ui_init_request(http_data_t *http_data);

   nhpc_status_t web_ui_register(fnc_ptr_nhpc_t func_trigger);
   
   nhpc_status_t web_ui_generate(web_ui_elements_t *web_ui_elements, char *file_path);
};

#endif