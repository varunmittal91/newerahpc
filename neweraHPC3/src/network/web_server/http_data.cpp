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

#include <include/http.h>
#include <include/file.h>
#include <include/strings.h>

using namespace std;

namespace neweraHPC
{
   void http_data_init(http_data_t **http_data)
   {
      (*http_data) = new http_data_t;
      memset((*http_data), 0, sizeof(http_data_t));
   }
   
   void http_data_destruct(http_data_t *http_data)
   {
      if(http_data_check_opt(http_data, HTTP_OPT_DATA_SET))
      {
	 http_content_t *content;
	 
	 if(http_data_check_opt(http_data, HTTP_OPT_PRE_ALLOCATED))
	    content = (*http_data).content.pre_allocated;
	 else 
	 {
	    content = (*http_data).content.new_allocated;
	    delete[] (char *)((*content).content_data);
	 }
	 http_content_destruct(content);
      }      
      
      if(http_data->request)
	 delete[] (http_data->request);      
      
      delete http_data;
   }
   
   nhpc_status_t http_data_prepare(http_data_t **http_data, nhpc_socket_t *socket)
   {      
      nhpc_status_t nrv = NHPC_SUCCESS;

      rbtree      *headers = socket->headers;
      const char  *command;
      string_t    *command_str;
      const char  *request_str;
      const char  *request_type_str;
      int          request_type;
      int          pos;
      
      const char  *content_length_str;
      const char  *content_type;
      const char  *user_agent;
      const char  *host;
      const char  *location;
      
      command = (const char *)(*headers).search("command");
      if(!command)
	 return NHPC_FAIL;
      
      command_str = nhpc_substr(command, ' ');
      if(command_str->count != 3)
      {
	 nrv = NHPC_FAIL;
	 goto return_status;
      }

      request_str      = command_str->strings[1];
      request_type_str = command_str->strings[0];
      
      if(nhpc_strcmp(request_type_str, "GET") == NHPC_SUCCESS)
         request_type = HTTP_REQUEST_GET;
      else if(nhpc_strcmp(request_type_str, "POST") == NHPC_SUCCESS)
         request_type = HTTP_REQUEST_POST;
      else 
      {
         nrv = NHPC_FAIL;
	 goto return_status;
      }
      
      if((pos = nhpc_strfind(request_str, '?')))
      {
	 char *tmp_request_str = nhpc_substr(request_str, 1, pos - 1);
	 
	 const char *tmp_str = request_str + pos;
	 if(*tmp_str != '\0')
	 {
	    cout << "Get request:" << tmp_str << endl;
	 }
	 
	 request_str = tmp_request_str;
      }
      else 
	 nhpc_strcpy((char **)&request_str, request_str);
            
      
      content_length_str = (const char *)(*headers).search("Content-Length");
      content_type       = (const char *)(*headers).search("Content-Type");
      user_agent         = (const char *)(*headers).search("User-Agent");
      location           = (const char *)(*headers).search("Location");
      host               = (const char *)(*headers).search("host");
      
      http_data_init(http_data);
      http_data_set_type(*http_data, request_type);
      
      (**http_data).socket         = socket;
      (**http_data).request        = request_str;
      (**http_data).content_type   = content_type;
      (**http_data).user_agent     = user_agent;
      (**http_data).location       = location;
      (**http_data).host           = host;
      if(content_length_str)
	 (**http_data).content_length = nhpc_strtoi(content_length_str);
      
   return_status:
      nhpc_string_delete(command_str);
      
      return nrv;
   }
   
   nhpc_status_t http_data_add_content(http_data_t *http_data, void *address, char *mime, 
				       nhpc_size_t len, int type, int opt)
   {
      char           *tmp_address = NULL;
      http_content_t *content;
      nhpc_status_t   nrv;

      bool delete_address = false;
      
      http_content_init(&content);      

      if(content_is_dir(type) || content_is_file(type))
      {
	 opt = HTTP_OPT_NEW_ALLOCATED;
	 
	 const char *request = (const char *)address;
	 
	 nrv = nhpc_fileordirectory(request);
	 if(nrv == NHPC_DIRECTORY)
	 {
	    const char *tmp_request;
	    
	    tmp_request = nhpc_strconcat(request, "/", HTTP_DEFAULT_FILE);
	    if((nrv = nhpc_fileordirectory(tmp_request)) != NHPC_FILE)
	    {
	       delete[] tmp_request;
	       tmp_request = request;
	       nrv         = NHPC_DIRECTORY;
	    }
	    else 
	       delete_address = true;
	       
	    address = (void *)tmp_request;
	    len = strlen(tmp_request) + 1;
	 }
	 else if(nrv == NHPC_FILE)
	 {
	    len = strlen(request) + 1;
	 }

	 if(nrv == NHPC_DIRECTORY)
	    type = HTTP_CONTENT_TYPE_DIR;
	 else if(nrv == NHPC_FILE)
	    type = HTTP_CONTENT_TYPE_FILE;
	 else 
	 {
	    type = HTTP_CONTENT_TYPE_MEM;
	    
	    http_status_set_status(http_data, HTTP_STATUS_NOTFOUND);
	    opt = HTTP_OPT_PRE_ALLOCATED;
	 }
      }
      
      if(!(http_status_check_status(http_data, HTTP_STATUS_OK)))
      {
	 address = (void *)HTTP_ERROR_PAGE;
	 len     = strlen(HTTP_ERROR_PAGE) + 1;
      }
      
      if(opt == HTTP_OPT_NEW_ALLOCATED)
      {
	 ((*http_data).content.new_allocated) = content;

	 tmp_address = new char [len];
	 memcpy(tmp_address, address, len);
      }
      else 
      {
	 ((*http_data).content.pre_allocated) = content;

	 tmp_address = (char *)address;
      }
      
      if(content_is_file(type))
	 nhpc_file_size(tmp_address, &len);
      else if(content_is_dir(type))
	 len = 0;
      
      http_content_set_type(content, type);
      (*content).content_data = tmp_address;
      if(mime)
	 nhpc_strcpy((char **)&((*content).content_mime), mime);
      (*content).len          = len;
      
      http_data_set_opt(http_data, HTTP_OPT_DATA_SET);
      http_data_set_opt(http_data, opt);

      if(delete_address)
	 delete[] (char *)address;
      
      return NHPC_SUCCESS;
   }
};