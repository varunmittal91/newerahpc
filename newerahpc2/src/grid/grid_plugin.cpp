//Copyright 2010 Varun Mittal ©
//newerahpc program is distributed under the terms of the GNU General Public License
//
//This file is part of newerahpc.
//
//newerahpc is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation version 2 of the License.
//
//newerahpc is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with newerahpc.  If not, see <http://www.gnu.org/licenses/>.

#include <network.h>
#include <dlfcn.h>

namespace newera_network{
	plugin_manager::plugin_manager(){
		mutex = new pthread_mutex_t;	
	}
	plugin_manager::~plugin_manager(){
		delete mutex;
	}
	void plugin_manager::add_request(conn_rec *in_rec,char *plg_name,char *plg_code){
		lock_plugin();
		
		unlock_plugin();
	}
	void plugin_manager::lock_plugin(){
		
	}
	void plugin_manager::unlock_plugin(){
		
	}
	void plugin_manager::wait_plugin(){
		
	}
	void plugin_manager::load(char *file_name){
		lock_plugin();
		void *dll = dlopen(file_name,RTLD_NOW);
		if(!dll){
			cout<<dlerror()<<endl;
			unlock_plugin();
			return;
		}
		func_ptr func_ptr_t = (func_ptr)dlsym(dll,"plugin_init");
		func_ptr func_ptr_t_c = (func_ptr)dlsym(dll,"plugin_exec_client");
		func_ptr func_ptr_t_p = (func_ptr)dlsym(dll,"plugin_processor");
		if(!func_ptr_t&&!func_ptr_t_c&&!func_ptr_t_p){
			cout<<dlerror()<<endl;
			unlock_plugin();
			return;
		}
		else{
			void *func_string_temp;
			func_string_temp = func_ptr_t(NULL);
			string *func_string = (string *)func_string_temp;
			if(check_dll((char*)(*func_string).c_str())==true)cout<<"dll already found"<<endl;
			if(check_dll((char*)(*func_string).c_str())==true){
				unlock_plugin();
				return;
			}
			func_ptr_t = (func_ptr)dlsym(dll,"plugin_exec");
			if(!func_ptr_t){
				unlock_plugin();
				return;
			}
			else{
				func_details *func_details_t = new func_details;
				func_details_t->ptr = func_ptr_t;
				func_details_t->ptr_client = func_ptr_t_c;
				func_details_t->ptr_processor = func_ptr_t_p;
				func_details_t->path = file_name;
				functions[*func_string] = func_details_t;
				cout<<"dll loaded exec function found"<<endl;
			}
		}
		unlock_plugin();
	}	
	bool plugin_manager::check_dll(char *func_name){
		functions_map::iterator check = functions.find(func_name);
		if(check==functions.end())return false;
		else return true;
	}	
};
