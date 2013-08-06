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
#include <fstream>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <neweraHPC/network.h>
#include <neweraHPC/general.h>
#include <neweraHPC/constants.h>
#include <neweraHPC/json.h>

#include <include/webui.h>

using namespace std;

namespace neweraHPC
{
   rbtree *app_handlers;
   const char *ui_temp_dir;
   
   nhpc_status_t web_ui_init()
   {
      nhpc_status_t nrv;
      
      app_handlers = new rbtree(RBTREE_STR);
      
      http_handler_register("app", (fnc_ptr_nhpc_t)web_ui_handler);
      
      nrv = nhpc_create_tmp_file_or_dir(&ui_temp_dir, HTTP_ROOT, NHPC_DIRECTORY, "jdesktop");
      
      return nrv;
   }
   
   nhpc_status_t web_ui_register(fnc_ptr_nhpc_t fnc_init)
   {
      app_details_t *app_details = new app_details_t;
      memset(app_details, 0, sizeof(app_details_t));
      nhpc_status_t  nrv;
      
      nrv = fnc_init(app_details);
      
      if(nrv != NHPC_SUCCESS)
      {
	 delete app_details;
	 return nrv;
      }
      
      {
	 json_t *app_attributes = new json_t;

	 (*app_attributes).add_element(JSON_STRING, "appname", web_ui_get_app_name(app_details));
	 (*app_attributes).add_element(JSON_STRING, "apptype", web_ui_get_app_type_string(app_details));
	 if(web_ui_opt_is_system_utility(app_details))
	    (*app_attributes).add_element(JSON_TRUE, "SystemUtility");
	 if(web_ui_opt_is_multiple_disabled(app_details))
	    (*app_attributes).add_element(JSON_TRUE, "MultipleDisabled");
	 (*app_attributes).close_element();	
	 
	 app_details->app_attributes = app_attributes;
      }

      rbtree *instances = new rbtree(RBTREE_NUM);
      app_details->instances = instances;
      
      (*app_handlers).insert(app_details, app_details->app_name);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t web_ui_init_app(app_details_t *app_details)
   {
      //app_details->func_trigger_local();
   }
   
   nhpc_status_t web_ui_handler(http_data_t *http_data)
   {
      app_details_t *app_details;
      web_ui_check_app(&app_details, http_data);
      if(!app_details)
      {
	 int url_pos = nhpc_strfind(http_data->request_page, '/', 2);
	 char *base_url = nhpc_substr(http_data->request_page, url_pos + 1, strlen(http_data->request_page));
	 char *new_request_page = nhpc_strconcat("/nuiDesktop/", base_url);
	 
	 nhpc_string_delete(http_data->request_page);
	 http_data->request_page = new_request_page;
	 
         nhpc_string_delete(base_url);	 

	 return NHPC_SUCCESS;
      }
      
      if(!(http_data->request_get))
      {
	 delete[] (http_data->request_page);
	 http_data->request_page = nhpc_strconcat("/nuiDesktop", "/standard.html");	     
      }
      else 
      {
	 
      }
      
      /*
      if(nhpc_strcmp(http_data->request_page, "*.json") == NHPC_SUCCESS)
      {
	 int str_pos = nhpc_strfind(http_data->request_page, '.');
	 
	 char *request_page = nhpc_substr(http_data->request_page, 1, str_pos - 1);
	 delete[] (http_data->request_page);
	 http_data->request_page = request_page;
	 
	 web_ui_init_request(http_data);
      }
      else if(nhpc_strcmp(http_data->request_page, "*.*") == NHPC_SUCCESS)
      {
	 int url_pos = nhpc_strfind(http_data->request_page, '/', 2);
	 char *base_url = nhpc_substr(http_data->request_page, url_pos + 1, strlen(http_data->request_page));
	 char *new_request_page = nhpc_strconcat("/nuiDesktop/", base_url);
	 
	 nhpc_string_delete(http_data->request_page);
	 http_data->request_page = new_request_page;
	 
         nhpc_string_delete(base_url);
      }
      else if(web_ui_check_app(&app_details, http_data) == NHPC_SUCCESS)
      {
	 if(http_data->request_type == HTTP_REQUEST_POST)
	 {
	    json_t *event = new json_t;
	    (*event).build_structure(http_data->sock->partial_content);
	    (*event).print();
	    cout << "Received event request" << endl;
	 }
	 else 
	 {
	    delete[] (http_data->request_page);
	    http_data->request_page = nhpc_strconcat("/nuiDesktop", "/standard.html");	    
	 }
      }
      else 
      {
	 delete[] (http_data->request_page);
	 http_data->request_page = nhpc_strconcat("/nuiDesktop", "/standard.html");
      }
       */
   }
   
   nhpc_status_t web_ui_check_app(app_details_t **app_details, http_data_t *http_data)
   {
      cout << http_data->request_page << endl;
      
      string_t *app_details_str = nhpc_substr(http_data->request_page, '/');
      if(app_details_str->count < 2)
      {
	 nhpc_string_delete(app_details_str);
	 return NHPC_FAIL;
      }
      
      cout << "searching:" << app_details_str->strings[1] << endl;
      (*app_details) = (app_details_t *)app_handlers->search(app_details_str->strings[1]);
      
      if(!(*app_details))
      {
	 nhpc_string_delete(app_details_str);
	 return NHPC_FAIL;
      }      
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t web_ui_init_request(http_data_t *http_data)
   {
      nhpc_socket_t *sock = http_data->sock;
      rbtree *ui_details;
      char *file_path;
      
      string_t *app_details_str = nhpc_substr(http_data->request_page, '/');
      if(app_details_str->count < 2)
      {
	 nhpc_string_delete(app_details_str);
	 return NHPC_FAIL;
      }
      
      app_details_t *app_details = (app_details_t *)app_handlers->search(app_details_str->strings[1]);
      
      if(!app_details)
      {
	 nhpc_string_delete(app_details_str);
	 return NHPC_FAIL;
      }
      
      nhpc_strcpy(&(http_data->custom_response_mime), "Content-Type: application/json");
      
      int instance_count = app_details->instances->length();
      app_instance_t *app_instance = new app_instance_t;
      app_instance->instance_id = instance_count + 1;
      app_details->instances->insert(app_instance);
      
      char *app_name = app_details_str->strings[1];
      
      web_ui_elements_t *web_ui_elements = app_details->web_ui_elements;
      
      web_ui_elements->http_data = http_data;
      web_ui_elements->instance_id = instance_count;
      
      if(!(web_ui_elements->is_ready))
      {
	 web_ui_elements->elements->add_element(JSON_STRING, "appname", app_name);
	 web_ui_elements->elements->add_element(JSON_OBJECT, "app_attributes");
	 web_ui_elements->elements->add_element(JSON_STRING, "instance_id", nhpc_itostr(instance_count));
	 //web_ui_elements->elements->add_element(JSON_STRING, "apptype", app_details->app_type);
	 web_ui_elements->elements->close_element();      
      }
      else 
      {
	 (*(web_ui_elements->elements))["app_attributes"];
	 (*(web_ui_elements->elements)).update("instance_id", nhpc_itostr(instance_count));
      }
      
      //(*(app_details->func_trigger_local))(http_data, web_ui_elements);
      
      web_ui_generate(web_ui_elements, file_path);
      
      nhpc_string_delete(app_details_str);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t web_ui_generate(web_ui_elements_t *web_ui_elements, char *file_path)
   {
      web_ui_elements->http_data->custom_response_data = (char *)web_ui_elements->elements->get_string();
      web_ui_elements->http_data->custom_response_type = NHPC_FILE;
   }
   
   web_ui_elements_t::web_ui_elements_t()
   {
      elements = new json_t;
      elements_tree = new rbtree(RBTREE_STR);
   }
   
   nhpc_status_t web_ui_elements_t::add_element(const char *element, const char *property)
   {
   }
   
   nhpc_status_t web_ui_elements_t::add_public_data()
   {
      return NHPC_SUCCESS;
   }
};