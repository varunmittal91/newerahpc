/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <include/neweraHPC.h>

using namespace std;

//const char *NHPC_HTTP_GET_HEADER = "HTTP/1.1 

void nhpc_http_prepare_response(nhpc_http_request_t *http_request) {
   cout << "preparing response data and buffer" << endl;
   
   //const char *header
   
   
   const char *test_mssg = "THIS IS A TEST hEADER";
   if(http_request->status.request_type & NHPC_HTTP_INVALID_REQUEST) {
      cout << "We have an invalid request" << endl;
   } else if(http_request->status.request_type & NHPC_HTTP_GET_REQUEST) {
      cout << "We have a get request" << endl;
   } else if(http_request->status.request_type & NHPC_HTTP_POST_REQUEST) {
      cout << "We have a post request" << endl;
   } else {
      cout << "Method nt implemnted";
   }
   
   
   nhpc_buffer_add_data(http_request->response_buffer, (u_char *)test_mssg, strlen(test_mssg), NHPC_BUFFER_DATA_MEM_BLOCK, 0);
}