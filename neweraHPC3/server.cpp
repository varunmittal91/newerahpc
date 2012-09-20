/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
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

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/neweraHPC.h>

using namespace std;
using namespace neweraHPC;

nhpc_status_t func_trigger(nhpc_socket_t *sock, web_ui_elements_t *web_ui_elements)
{
   cout<<"test app trigger invocated"<<endl;
   
   web_ui_elements->add_element("connect", "test_app2");
   web_ui_elements->add_element("window", "window");
   web_ui_elements->add_element("height", "40%");
   web_ui_elements->add_element("width", "40%");
   web_ui_elements->add_element("button1", "about");
   web_ui_elements->add_element("button2", "help");
   web_ui_elements->add_public_data();
   
   return NHPC_SUCCESS;
}

void test_json()
{
   nhpc_json_t nhpc_json;
   
   nhpc_json.add_element(JSON_STRING, "firstName", "varun");
   nhpc_json.add_element(JSON_STRING, "lastName", "dhawan");

   nhpc_json.add_element(JSON_OBJECT, "address");
      nhpc_json.add_element(JSON_STRING, "streetAddress", "sector 1");
      nhpc_json.add_element(JSON_NUMBER, "houseNumber", "420");
   nhpc_json.close_element();

   nhpc_json.add_element(JSON_ARRAY, "addresses");
      nhpc_json.add_element(JSON_OBJECT);
	 nhpc_json.add_element(JSON_STRING, "streetAddress", "sector 1");
	 nhpc_json.add_element(JSON_NUMBER, "houseNumber", "420");
      nhpc_json.close_element();
      nhpc_json.add_element(JSON_OBJECT);
	 nhpc_json.add_element(JSON_STRING, "streetAddress", "sector 2");
	 nhpc_json.add_element(JSON_NUMBER, "houseNumber", "421");
      nhpc_json.close_element();
   nhpc_json.close_element();

   nhpc_json.add_element(JSON_ARRAY, "numbers2");
      nhpc_json.add_element(JSON_NUMBER, "Number", "420");
      nhpc_json.add_element(JSON_NUMBER, "Number", "421");
   nhpc_json.close_element();   
   
   nhpc_json.add_element(JSON_OBJECT, "complex");
      nhpc_json.add_element(JSON_OBJECT, "complex_in");
	 nhpc_json.add_element(JSON_ARRAY, "complex_in_in");
	    nhpc_json.add_element(JSON_NUMBER, "1", "12");
	    nhpc_json.add_element(JSON_NUMBER, "2", "12");
	 nhpc_json.close_element();
	 nhpc_json.add_element(JSON_ARRAY, "complex_in_in2");
	    nhpc_json.add_element(JSON_TRUE, "value1");
	    nhpc_json.add_element(JSON_TRUE, "value2");
	 nhpc_json.close_element();
      nhpc_json.close_element();
   nhpc_json.close_element();
   
   nhpc_json.add_element(JSON_OBJECT, "complex1");
   nhpc_json.close_element();
   
   nhpc_json.close_element();
   
   nhpc_json.print();
   char *json_string = nhpc_json.get_stream();
   cout<<json_string<<endl;

   ifstream json_test_file("/tmp/test.json");
   string json_string2, line;
   while(getline(json_test_file, line))
   {
      json_string2 += line;
   }
   cout<<json_string2<<endl;
   
   nhpc_json_t *test = new nhpc_json_t;
   test->build_structure(json_string2.c_str());
   cout<<test->get_stream()<<endl;
   
   //test->print();
}

int main(int argc, char **argv)
{
   neweraHPC_init(argc, argv);

   test_json();
   
   nhpc_status_t nrv;
   
   http_init();
   web_ui_init();
   web_ui_register("neweraHPC", (fnc_ptr_nhpc_two_t)func_trigger);

   nhpc_grid_server_t grid_server;
   nrv = grid_server.grid_server_init();
   
   if(nrv != NHPC_SUCCESS)
      LOG_ERROR("Grid initialization failed");
   
   return 0;
}
