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

const char        *NHPC_HTTP_HEADER    = "HTTP/1.1 ";
const nhpc_size_t  nhpc_http_headerlen = strlen(NHPC_HTTP_HEADER);

void nhpc_http_prepare_response(nhpc_http_request_t *http_request) {
   
   nhpc_size_t status_strlen = strlen((const char *)http_request->status.status_str);
   nhpc_size_t header_strlen = status_strlen + nhpc_http_headerlen + 3;
   
   u_char *header_str = (u_char *)nhpc_palloc(http_request->pool, header_strlen);
   memcpy(header_str, NHPC_HTTP_HEADER, nhpc_http_headerlen);
   memcpy(header_str + nhpc_http_headerlen, http_request->status.status_str, status_strlen);
   header_str[header_strlen - 3] = '\r';
   header_str[header_strlen - 2] = '\n';
   header_str[header_strlen - 1]     = '\0';
			
   nhpc_buffer_add_data(http_request->response_buffer, (u_char *)header_str, header_strlen,
			NHPC_BUFFER_DATA_MEM_BLOCK, 0);
}