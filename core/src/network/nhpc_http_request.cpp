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

nhpc_size_t http_root_strlen;

const char *temp_error_response = "<html><head><title>Content Not Found</title></head><body><center><h1>404 - Content Not Found</h1></center></body></html>";
const char *temp_error_invalid  = "<html><head><title>Bad Request</title></head><body><center><h1>400 - Bad Request</h1></center></body></html>";

nhpc_http_request_types http_request_types[] = {
   "GET " , NHPC_HTTP_GET_REQUEST,
   "POST ", NHPC_HTTP_POST_REQUEST,
};

void nhpc_http_handler(nhpc_event_t *ev) {
   nhpc_connection_t    *c  = (nhpc_connection_t *)ev->data;
   nhpc_communication_t *cm = c->communication;

   nhpc_http_request_t  *http_request;
   if(!cm->data) {
      http_request = nhpc_http_init_request_data(c->pool, cm);
      cm->data     = http_request;
   } else {
      http_request = (nhpc_http_request_t *)cm->data;
   }
   
   http_request->response_buffer = nhpc_buffer_init(http_request->pool);

   if(http_request->status.request_type & NHPC_HTTP_INVALID_REQUEST) {
      http_request->status.status_code = NHPC_HTTP_STATUS_BAD_REQUEST;
      http_request->status.status_str  = (const u_char *)"400 BAD REQUEST";
   } else {
      if(0) {
	 /* the application handler routine  will be placed here */   
      } else {
	 /* prepare response based on file/dir check routine */
      
	 nhpc_size_t  requestlen = strlen(http_request->request_str);
	 u_char      *request    = (u_char *)nhpc_palloc(http_request->pool, requestlen + http_root_strlen + 1);
	 memcpy(request, HTTP_ROOT, http_root_strlen);
	 memcpy(request + http_root_strlen, http_request->request_str, requestlen);
	 request[requestlen + http_root_strlen] = '\0';
      
	 nhpc_status_t nrv = nhpc_fileordirectory((const char *)request);
      
	 if(nrv & NHPC_FILE_NOT_FOUND) {
	    http_request->status.status_code = NHPC_HTTP_STATUS_NOT_FOUND;
	    http_request->status.status_str  = (const u_char *)"404 NOT FOUND";
	 } else {
	    http_request->status.status_code = NHPC_HTTP_STATUS_OK;
	    http_request->status.status_str  = (const u_char *)"200 OK";
	 }
      }
   }
   
   nhpc_http_prepare_response(http_request);

   nhpc_shutdown_connection(c, SHUT_RD);
   nhpc_add_event(c->wev, NHPC_WRITE_EVENT, 0);
   c->wev->handler = nhpc_http_write_handler;
   c->wev->handler(c->wev);
}

void nhpc_http_write_handler(nhpc_event_t *ev) {
   nhpc_connection_t    *c  = (nhpc_connection_t *) ev->data;
   nhpc_communication_t *cm = c->communication;
   
   nhpc_http_request_t *http_request = (nhpc_http_request_t *) cm->data;
   nhpc_buffer_t *buffer = http_request->response_buffer;
   
   nhpc_size_t datasent = (buffer->end - buffer->start);
   nhpc_status_t nrv = nhpc_send(c, (char *)buffer->start, &datasent);
   
   if(nrv == NHPC_EOF) {
      nhpc_accept_close_connection(c);      
   } else if(nrv == NHPC_SUCCESS) {
      if((datasent = (buffer->end - buffer->start) - datasent) > 0) {
	 buffer->start += datasent;
      } else if(datasent == 0) {
	 if(buffer->chain) {
	    buffer->start = buffer->chain->start;
	    buffer->end   = buffer->chain->end;
	    
	    buffer->chain = buffer->chain->next;
	 } else {
	    nhpc_accept_close_connection(c);      
	    return;	    
	 }
	 cout << "Length zero" << endl;
      } 
      
      /*
      else {
	 if(buffer->chain) {
	    buffer->start = buffer->chain->start;
	    buffer->end   = buffer->chain->end;
	    
	    buffer->chain = buffer->chain->next;
	 }
	 else {
	    nhpc_accept_close_connection(c);      
	    return;
	 }
      }
      */
   }
}

nhpc_http_request_t *nhpc_http_init_request_data(nhpc_pool_t *p, nhpc_communication_t *cm) {
   char *command_str = cm->command_str;
   
   nhpc_http_request_t *http_request = (nhpc_http_request_t *)nhpc_palloc(p, sizeof(nhpc_http_request_t));
   http_request->pool = p;
   
   switch(*command_str) {
      case 'G':
	 http_request->status.request_type = NHPC_HTTP_GET_REQUEST;
	 command_str += 4;
	 break;
      case 'P':
	 http_request->status.request_type = NHPC_HTTP_POST_REQUEST;
	 command_str += 5;
	 break;
   };

   int space_pos = nhpc_strfind(command_str, ' ');
   char *request_str  = command_str;
   char *http_version = command_str + space_pos + 5;
   request_str[space_pos - 1] = '\0';
   
   int   header_count = nhpc_rbtree_length(cm->headers);
   char *header_str;
   
   for(int i = 1; i <= header_count; i++) {
      header_str = (char *)(nhpc_rbtree_search(cm->headers, i));
      if(!header_str) {
	 http_request->status.request_type |= NHPC_HTTP_INVALID_REQUEST;
	 goto return_http_request;
      }
   }
   
   if(nhpc_http_parse_request_str(http_request, request_str) != NHPC_SUCCESS) {
      http_request->status.request_type |= NHPC_HTTP_INVALID_REQUEST;
      goto return_http_request;
   }
   nhpc_http_parse_http_version(http_request, http_version);
   
return_http_request:
   
   return http_request;
}