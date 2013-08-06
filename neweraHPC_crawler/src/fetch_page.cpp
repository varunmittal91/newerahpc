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

#include <fstream>
#include <iostream>

#include <neweraHPC/network.h>
#include <neweraHPC/http.h>

#include <include/fetch_page.h>
#include <include/url.h>

using namespace std;
using namespace neweraHPC;

namespace search
{
   FetchPage::FetchPage(const char *src_webpage, const char *output_file) 
   {
      nhpc_strcpy((char **)&_src_webpage, src_webpage);
      nhpc_strcpy((char **)&_output_file, output_file);
   }
   FetchPage::~FetchPage()
   {
      
   }

   void FetchPage::get()
   {
      const char *location;
      const char *parent_location = NULL;
      nhpc_strcpy((char **)&location, _src_webpage);
      
      nhpc_status_t  nrv;      
      const char    *file_name = NULL;
      
   fetch_page:
      nhpc_socket_t *sock = NULL;
      http_data_t   *http_data = new http_data_t;
      
      string_t *url = getURL(location, parent_location);
      if(!url)
	 return;
      
      cout << location;
      if(parent_location)
	 cout << ":" << parent_location;
      cout << endl;
      
      nrv = http_get_file(http_data, &file_name, &sock, _URL_get_request(url), _URL_get_domain(url), _URL_get_port(url), _output_file);
      if(nrv != NHPC_SUCCESS)
      {
	 cout << nrv << endl;
	 cout << "unsuccessful" << endl;
	 goto return_page;
      }
      if(http_data->response_code == 301 || http_data->response_code == 302)
      {
	 const char *tmp_location;
	 
	 nhpc_strcpy((char **)&tmp_location, http_data->location);
	 parent_location = location;
	 location = tmp_location;

	 delete_http_headers(http_data);
	 delete http_data;
	 goto fetch_page;
	 nhpc_string_delete(url);
      }	 
      
   return_page:      
      delete_http_headers(http_data);
      delete http_data;
      return; 
   }
};
