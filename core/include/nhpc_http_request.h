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

#ifndef _NHPC_HTTP_REQUEST_H_
#define _NHPC_HTTP_REQUEST_H_

typedef void (*nhpc_http_handler_ptr)(nhpc_http_request_t *);

#define NHPC_HTTP_GET_REQUEST     0x0001
#define NHPC_HTTP_POST_REQUEST    0x0002
#define NHPC_HTTP_INVALID_REQUEST 0x0004

struct nhpc_http_request_types {
   const char  *str;
   nhpc_uint_t  value;
};
extern nhpc_http_request_types http_request_types[];

struct nhpc_http_status_s {
   unsigned       version;
   nhpc_uint_t    status_code;
   nhpc_uint_t    request_type;
};

struct nhpc_http_request_s {
   nhpc_connection_t       *c;
   nhpc_communication_t    *communication;
   
   nhpc_http_handler_ptr    read_handler;
   nhpc_http_handler_ptr    write_handler;
   
   nhpc_http_status_s       nhpc_http_status_t;
   
   unsigned                 read:1;
   unsigned                 write:1;
};

void nhpc_http_handler(nhpc_event_t *ev);

#endif