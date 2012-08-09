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
#include <include/network.h>
#include <include/general.h>
#include <include/web_ui.h>
#include <include/constants.h>

using namespace std;

namespace neweraHPC
{
   rbtree_t *app_handlers;
   const char *ui_temp_dir;
   
   nhpc_status_t web_ui_init()
   {
      app_handlers = new rbtree_t(NHPC_RBTREE_STR);
      
      nhpc_status_t nrv = nhpc_create_tmp_file_or_dir(&ui_temp_dir, HTTP_ROOT, NHPC_DIRECTORY, "ui_temp");
      
      return nrv;
   }
   
   nhpc_status_t web_ui_register(const char *app_name, fnc_ptr_nhpc_two_t func_trigger)
   {
      fnc_ptr_nhpc_two_t *func_trigger_local = new fnc_ptr_nhpc_two_t;
      (*func_trigger_local) = func_trigger;
      
      nhpc_status_t rv = app_handlers->insert(func_trigger_local, app_name);
      
      return rv;
   }
   
   nhpc_status_t web_ui_init_request(nhpc_socket_t *sock, string_t *request, rbtree_t **ui_details, char **file_path)
   {
      string_t *app_details = nhpc_substr(request->strings[1], '/');
      if(app_details->count < 2)
      {
	 return NHPC_FAIL;
      }
      
      fnc_ptr_nhpc_two_t *func_trigger_local = (fnc_ptr_nhpc_two_t *)app_handlers->search(app_details->strings[1]);
      
      if(!func_trigger_local)
      {
	 return NHPC_FAIL;
      }
      
      nhpc_create_tmp_file_or_dir((const char **)file_path, ui_temp_dir, NHPC_FILE, app_details->strings[1]);
      
      web_ui_elements_t *web_ui_elements = new web_ui_elements_t(ui_temp_dir, (*file_path));
      web_ui_elements->add_element("app_title", app_details->strings[1]);
      
      (*func_trigger_local)(sock, web_ui_elements);
      
      web_ui_generate(web_ui_elements, (*file_path));
      
      delete[] (*file_path);
      (*file_path) = nhpc_strconcat(ui_temp_dir, "/standard.html");
      cout<<(*file_path)<<endl;
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t web_ui_generate(web_ui_elements_t *web_ui_elements, char *file_path)
   {
      ofstream xml_file(file_path);
      
      rbtree_t *elements = web_ui_elements->elements;
      char *element;
      
      xml_file<<"<neweraHPC_ui>"<<endl;
      
      element = (char *)elements->search("app_title");
      if(element)
      {
	 xml_file<<"<app_title>"<<element<<"</app_title>"<<endl;
      }
      element = (char *)elements->search("window");
      if(element)
      {
	 xml_file<<"<window>"<<endl;
	 
	 element = (char *)elements->search("height");
	 if(element)
	 {
	    xml_file<<"<height>"<<element<<"</height>"<<endl;
	 }
	 element = (char *)elements->search("width");
	 if(element)
	 {
	    xml_file<<"<width>"<<element<<"</width>"<<endl;
	 }
	 
	 xml_file<<"</window>"<<endl;
      }
      
      xml_file<<"</neweraHPC_ui>"<<endl;
      
      xml_file.close();
   }
   
   web_ui_elements_t::web_ui_elements_t(const char *_working_dir, const char *_app_xml)
   {
      working_dir = nhpc_strconcat(_working_dir, "/");
      
      string_t *temp = nhpc_substr(_app_xml, '/');
      elements = new rbtree_t(NHPC_RBTREE_STR);
      
      nhpc_strcpy(&app_xml, temp->strings[temp->count - 1]);
   }
   
   nhpc_status_t web_ui_elements_t::add_element(const char *element, const char *property)
   {
      if(nhpc_strcmp(element, "connect") == NHPC_SUCCESS)
      {
	 char *remote_app_xml_path = nhpc_strconcat(working_dir, property, ".public");
	 
	 elements->insert(remote_app_xml_path, "connect");
      }
      else 
      {
	 elements->insert((void *)property, element);
      }
   }
   
   nhpc_status_t web_ui_elements_t::add_public_data()
   {
      char *file_name = nhpc_strconcat(app_xml, ".public");
      nhpc_status_t nrv = nhpc_create_tmp_file_or_dir((const char **)&(app_xml_public), working_dir, NHPC_FILE, file_name);
   }
};