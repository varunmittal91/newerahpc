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
		request_count = 0;
		requests = NULL;
	}
	plugin_manager::~plugin_manager(){
		delete mutex;
		for(int cntr=0;cntr<request_count;cntr++){
			
		}
	}
	void plugin_manager::add_request(conn_rec *in_rec,char *plg_name,char *plg_code){
		lock_plugin();
		if(check_dll(plg_name)==true)return;
		if(check_request(plg_name,plg_code)==true)return;
		plugin_request *new_request = new plugin_request;
		new_request->host = in_rec->host;
		new_request->port = in_rec->port;
		new_request->plg_name = plg_name;
		new_request->plg_code = plg_code;
		new_request->next = NULL;
		if(request_count==0){
			requests = new_request;	
		}
		else{
			plugin_request *tmp_request = requests;
			while(tmp_request->next!=NULL){
				tmp_request = tmp_request->next;
			}
			tmp_request->next = new_request;
		}
		request_count++;
		unlock_plugin();
	}
	void plugin_manager::display_plugin_requests(){
		lock_plugin();
		plugin_request *tmp_request = requests;
		for(int cntr=0;cntr<request_count;cntr++){
			cout<<tmp_request->plg_name<<endl;
			tmp_request = tmp_request->next;
		}
		unlock_plugin();
	}
	bool plugin_manager::check_request(char *plg_name,char *plg_code){
		if(request_count==0)return false;
		plugin_request *tmp_request = requests;
		while(tmp_request!=NULL){
			if(find(tmp_request->plg_name,plg_name)!=STR_NPOS)return true;
			tmp_request = tmp_request->next;
		}
		return false;
	}
	void plugin_manager::lock_plugin(){
		
	}
	void plugin_manager::unlock_plugin(){
		
	}
	void plugin_manager::wait_plugin(){
		
	}
	void plugin_manager::load(char *file_name){
		if(find(file_name,(char *)".nxi")!=STR_NPOS||find(file_name,(char *)".info")!=STR_NPOS){
			file_name = load_nxi(file_name);
		}
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
		functions_map::iterator check = functions.find((char *)func_name);
		if(check==functions.end())return false;
		else return true;
	}	
};
