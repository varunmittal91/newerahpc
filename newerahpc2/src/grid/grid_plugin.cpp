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
		requests_que = new queue;
		(*requests_que) += (void *)mutex;
	}
	plugin_manager::~plugin_manager(){
		delete mutex;
		delete requests_que;
	}
	void plugin_manager::init_manager(){
		cout<<"initing plugin manager"<<endl;
		pthread_t thread;
		pthread_create(&thread,NULL,plugin_manager::dispatcher,(void *)requests_que);
	}
	void *plugin_manager::dispatcher(void *data){
		queue *requests_que = (queue *)data;
		pthread_mutex_t *mutex = (pthread_mutex_t *)(*requests_que)[0];
		while(1){
			sleep(2);
			pthread_mutex_lock(mutex);
			if(requests_que->count==PLUGIN_QUEUE_RESERVED){
				pthread_mutex_unlock(mutex);
				continue;
			}
			int cntr=PLUGIN_QUEUE_RESERVED;
			while(cntr<requests_que->count){
				plugin_request *tmp_request = (plugin_request *)(*requests_que)[cntr];
				conn_rec *out_rec = new conn_rec;
				out_rec->host = tmp_request->host;
				out_rec->port = tmp_request->port;
				connect(out_rec);
				if(!out_rec->conn_status)cout<<"connection failed"<<endl;
				else{
					network_write *write = new network_write(out_rec->sockfd);
					client_request *req_plugin = new client_request;
					req_plugin->get_file = GET_FILE;
					write->add("GRID_GETPLUGIN");
					write->add(tmp_request->plg_name);
					write->add(itoa(server_port));
					write->add("");
					write->push();
					shutdown(out_rec->sockfd,SHUT_RDWR);
					close(out_rec->sockfd);
				}
				cntr++;
				(*requests_que) -= tmp_request;
			}
			pthread_mutex_unlock(mutex);
		}
	}
	void plugin_manager::wait_plugin(char *plg_name,char *plg_code){
		while(1){
			if(hpc_data->check_dll(plg_name))break;
			sleep(1);
		}
	}
	std::string plugin_manager::return_path(char *plugin){
		lock_plugin();
		func_details *func_details = functions[plugin];
		if(func_details->path_nxi.length()>0){
			unlock_plugin();
			return func_details->path_nxi;
		}
		unlock_plugin();
		return func_details->path;
	}	
	void plugin_manager::add_request(char *host,int port,char *plg_name,char *plg_code,int status){
		lock_plugin();
		if(check_dll(plg_name)==true){
			unlock_plugin();
			return;
		}
		if(check_request(plg_name,plg_code)==true){
			unlock_plugin();
			if(status==WAIT_PLUGIN)wait_plugin(plg_name,plg_code);
			return;
		}
		plugin_request *new_request = new plugin_request;
		new_request->host = host;
		new_request->port = port;
		new_request->plg_name = plg_name;
		new_request->plg_code = plg_code;
		(*requests_que) += (void *)new_request;
		unlock_plugin();
		if(status==WAIT_PLUGIN)wait_plugin(plg_name,plg_code);
	}
	void plugin_manager::display_plugin_requests(){
		lock_plugin();
		plugin_request *tmp_request;
		for(int cntr=PLUGIN_QUEUE_RESERVED;cntr<requests_que->count;cntr++){
			tmp_request = (plugin_request *)(*requests_que)[cntr];
			cout<<tmp_request->plg_name<<endl;
		}
		unlock_plugin();
	}
	bool plugin_manager::check_request(char *plg_name,char *plg_code){
		lock_plugin();
		plugin_request *tmp_request;
		for(int cntr=PLUGIN_QUEUE_RESERVED;cntr<requests_que->count;cntr++){
			tmp_request = (plugin_request *)(*requests_que)[cntr];
			if(find(tmp_request->plg_name,plg_name)!=STR_NPOS){
				unlock_plugin();
				return true;
			}
		}
		unlock_plugin();
		return false;
	}
	void plugin_manager::lock_plugin(){
		
	}
	void plugin_manager::unlock_plugin(){
		
	}
	void plugin_manager::load(char *file_name){
		lock_plugin();
		func_details *func_details_t = new func_details;
		if(check_nxi(file_name)==true){
			func_details_t->path_nxi = file_name;
			std::string *file_name_nxi = load_nxi(func_details_t);
			if(file_name_nxi==NULL){
				delete func_details_t;
				unlock_plugin();
				return;
			}
			file_name = (char *)(*file_name_nxi).c_str();
		}
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
