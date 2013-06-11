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

#include <iostream>

#include <neweraHPC/strings.h>

#include <include/grid_http_interface.h>
#include <include/html_template.h>

using namespace std;

namespace neweraHPC
{
   void grid_http_init()
   {
      http_handler_register_app("grid", grid_http_handler);
   }
   
   void grid_http_handler(http_data_t *http_data)
   {
      cout << http_data->request << endl;
      http_data_add_content(http_data, (void *)grid_http_html_template, NULL, strlen(grid_http_html_template), 
			    HTTP_CONTENT_TYPE_MEM, HTTP_OPT_PRE_ALLOCATED);
   }
};