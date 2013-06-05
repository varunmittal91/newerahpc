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

//#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/error.h>
#include <include/strings.h>
#include <include/file.h>
#include <include/http.h>
#include <include/headers.h>
#include <include/sockets.h>
#include <include/network.h>
#include <include/neweraHPC.h>

namespace neweraHPC
{
   const char *request_type_strings[] = 
   {
      "HTTP_INVALID",
      "HTTP_REQUEST_GET",
      "HTTP_REQUEST_POST",
      "HTTP_RESPONSE_GET",
      "HTTP_RESPONSE_POST"
   };
   
   rbtree *http_handlers;
   void sig_action(int);
   
   nhpc_status_t http_handler_register(const char *handler_string, fnc_ptr_nhpc_t handler_function)
   {
      fnc_ptr_nhpc_t *func_trigger_local = new fnc_ptr_nhpc_t;
      (*func_trigger_local) = handler_function;
      
      nhpc_status_t rv = http_handlers->insert(func_trigger_local, handler_string);
      
      return rv;      
   }
   
   void http_init(network_t *network)
   {
      http_setup();
      
      LOG_INFO("Initialize http handler");
      http_handlers = new rbtree(RBTREE_STR);
      fnc_ptr_t http_handler = (fnc_ptr_t)http_request_handler;
      (*network).network_addons->insert((void *)http_handler, "HTTP");
   }
   
   void http_setup()
   {
      const char *test_proxy = nhpc_get_cmdline_argument("http-proxy");  
      
      if(test_proxy)
      {
	 int pos = nhpc_strfind(test_proxy, ':');
	 if(pos)
	 {
	    const char *host = nhpc_substr(test_proxy, 1, pos - 1);
	    const char *port = nhpc_substr(test_proxy, pos + 1, strlen(test_proxy));
	    http_proxy = new _http_proxy;
	    memset(http_proxy, 0, sizeof(_http_proxy));
	    http_proxy->host = host;
	    http_proxy->port = port;
	 }
      }
   }
   
   void http_request_handler(nhpc_socket_t *sock)
   {
      http_data_t *http_data = new http_data_t;
      memset(http_data, 0, sizeof(http_data_t));
      nhpc_status_t nrv = read_headers(sock->headers, &http_data);
      
      http_data->sock = sock;
      if(nrv == NHPC_SUCCESS)
      {
	 LOG_INFO("HTTP Request type: "<<request_type_strings[http_data->request_type]);
	 http_request(http_data);	 
      }
      
      delete_http_headers(http_data);
   }
   
   void http_request(http_data_t *http_data)
   {
      nhpc_socket_t *sock = http_data->sock;
      
      if((http_data->request_type) == HTTP_REQUEST_GET || (http_data->request_type) == HTTP_REQUEST_POST)
      {
	 if((http_data->request_type) == HTTP_REQUEST_POST)
	    LOG_INFO("Partial Content:" << sock->partial_content);
	 
	 string_t *tmp_str = nhpc_substr(http_data->request_page, '/');

	 if(tmp_str)
	 {
	    char *app_name = tmp_str->strings[0];
	 
	    LOG_INFO("Checking for: " << app_name);
	    fnc_ptr_nhpc_t *func_trigger_local = (fnc_ptr_nhpc_t *)http_handlers->search(app_name);
	    if(func_trigger_local != NULL)
	    {
	       LOG_INFO("Found http handler: " << app_name);
	       nhpc_status_t nrv = (*func_trigger_local)(http_data);
	    }
	    
	    nhpc_string_delete(tmp_str);
	 }
	 
	 char *file_path = NULL;
	 nhpc_size_t file_size;
	 nhpc_status_t nrv;
	 
	 if(!(http_data->custom_response_data))
	 {
	    file_path = nhpc_strconcat(HTTP_ROOT, http_data->request_page);
	    nrv = nhpc_file_size(file_path, &file_size);
	 }
	 else 
	 {	    	 
	    nrv = http_data->custom_response_type;
	 }
	 
	 if(nrv == NHPC_FILE_NOT_FOUND)
	 {
	    const char *mssg = "HTTP/1.1 404 Content Not Found\r\n\r\nContent Not Found\r\n";
	    nhpc_size_t size = strlen(mssg);
	    socket_send(sock, (char *)mssg, &size);
	 }
	 else if(nrv == NHPC_DIRECTORY)
	 {
	    const char *mssg = "HTTP/1.1 404 Content Not Found\r\n\r\nServer Doesn't Know How To Handle Directory\r\n";
	    nhpc_size_t size = strlen(mssg);
	    socket_send(sock, (char *)mssg, &size);
	 }
	 else 
	 {
	    nhpc_headers_t *headers = new nhpc_headers_t;
	    headers->insert("HTTP/1.1 200 OK");

	    char *file_size_str;
	    
	    if(!(http_data->custom_response_data))
	    {
	       file_size_str = nhpc_itostr(file_size);
	    
	       headers->insert("Content-Length", file_size_str);

	       /* Deciding mime types */
	       if(nhpc_strcmp(file_path, "*.json"))
		  headers->insert("Content-Type: application/json");
	       else if(nhpc_strcmp(file_path, "*.js"))
		  headers->insert("Content-Type: application/javascript");
	       else if(nhpc_strcmp(file_path, "*.css"))
		  headers->insert("Content-Type: text/css");
	       	    
	       headers->write(sock);
	    
	       delete headers;
	    
	       FILE *fp = fopen(file_path, "r");
	       if(!fp)
	       {
		  LOG_ERROR("Error opening file");
		  exit(0);
	       }

	       nhpc_status_t nrv;
	    
	       char buffer[10000];	    
	       nhpc_size_t len;
	    
	       do
	       {
		  bzero(buffer, sizeof(buffer));
		  len = fread(buffer, 1, sizeof(buffer), fp);
		  nrv = socket_sendmsg(sock, buffer, &len);
	       }while(!feof(fp) && nrv != EPIPE);

	       fclose(fp);
	    }
	    else 
	    {
	       file_size = strlen(http_data->custom_response_data);
	       file_size_str = nhpc_itostr(file_size);

	       headers->insert("Content-Length", file_size_str);
	       if(http_data->custom_response_mime)
		  headers->insert(http_data->custom_response_mime);
               headers->insert("Access-Control-Allow-Origin: *");
	       headers->write(sock);
	       delete headers;
	       
	       socket_sendmsg(sock, http_data->custom_response_data, &file_size);
	    }
	    
	    nhpc_string_delete(file_size_str);
	 }

	 if(file_path)
	    nhpc_string_delete(file_path);
      }
      else if((http_data->request_type) == HTTP_INVALID)
      {
	 const char *mssg = "HTTP/1.1 403 Invalid Request\r\n\r\nInvalid request\r\n";
	 nhpc_size_t size = strlen(mssg);
	 socket_send(sock, (char *)mssg, &size);	 
      }
   }
   
   nhpc_status_t http_get_file(http_data_t *http_data, const char **file_path, nhpc_socket_t **sock, const char *target_file, 
			       const char *host_addr, const char *host_port, const char *out_file)
   {
      nhpc_status_t nrv;

      if(!(*sock))
      {
	 if(http_proxy)
	 {
	    nrv = socket_connect(sock, http_proxy_host, http_proxy_port, AF_INET, SOCK_STREAM, 0);
	    if(nrv != NHPC_SUCCESS)
	    {
	       perror("connects");
	       return nrv;
	    }
	    
	    const char *host    = nhpc_strconcat(host_addr, ":", host_port);
	    const char *command = nhpc_strconcat("CONNECT ", host, " HTTP/1.1");
	    
	    nhpc_headers_t *headers = new nhpc_headers_t;
	    
	    headers->insert(command);
	    headers->insert("Host", host);
	    headers->write(*sock);

	    delete headers;
	    delete[] command;
	    delete[] host;
	    
	    char buffer[100];
	    nhpc_size_t size = sizeof(buffer);
	    bzero(buffer, sizeof(buffer));
	    nrv = socket_recv(*sock, buffer, &size);
	    
	    if(nrv != NHPC_SUCCESS)
	    {
	       socket_close(*sock);
	       socket_delete(*sock);
	       
	       return nrv;
	    }
	 }
	 else 
	 {
	    nrv = socket_connect(sock, host_addr, host_port, AF_INET, SOCK_STREAM, 0);
	    if(nrv != NHPC_SUCCESS)
	    {
	       perror("connects");
	       return nrv;
	    }
	 }
      }
            
      if(out_file)
	 *file_path = out_file;
      else 
	 nhpc_create_tmp_file_or_dir(file_path, "/tmp/neweraHPC", NHPC_FILE);
      
      const char *command = nhpc_strconcat("GET /", target_file, " HTTP/1.1");
      nhpc_headers_t *headers = new nhpc_headers_t;
      headers->insert(command);
      headers->insert("User-Agent: neweraHPC");
      headers->insert("Host", host_addr);
      headers->write(*sock);
      delete headers;
      nhpc_string_delete((char *)command);
      
      FILE *fp = fopen(*file_path, "w+");
      nhpc_size_t size;
      nhpc_size_t size_downloaded = 0;
      nhpc_size_t file_size = 0;
      
      char buffer[10000];
      nhpc_size_t header_size = 0;
      
      while((*sock)->have_headers != true)
      {
	 size = 10000;
	 bzero(buffer, size);
	 
	 nrv = socket_recv(*sock, buffer, &size);
	 nhpc_analyze_stream(*sock, buffer, &size, &header_size);
	 
	 if(nrv == NHPC_EOF)
	    break;
      }                  
      nhpc_display_headers(*sock);
      read_headers((*sock)->headers, &http_data);
      file_size = http_data->content_length;

      if((size - header_size) != 0)
      {
	 size_downloaded += (size - header_size);
	 fwrite((buffer + header_size), 1, (size - header_size), fp);
      }
      
      do 
      {
	 bzero(buffer, sizeof(buffer));
	 size = sizeof(buffer);
	 nrv = socket_recv(*sock, buffer, &size); 
	 fwrite(buffer, 1, size, fp);	 
	 size_downloaded += size;
      }while((nrv != NHPC_EOF) && file_size != size_downloaded);
      
      fclose(fp);
      
      if(nrv == EISCONN || nrv == NHPC_EOF)
	 nrv = NHPC_SUCCESS;
      
      socket_close(*sock);
      socket_delete(*sock);
      
      return nrv;
   }
   
   void http_response(nhpc_socket_t *sock)
   {
      
   }
};
