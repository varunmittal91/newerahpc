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
#include <map>
#include <dlfcn.h>

namespace newera_network{
	void *grid_execute_thread(void *data){
		hpc_data->execute((char *)data);
	}
	void grid_execute(char *func_name){
		pthread_t thread;
		pthread_create(&thread,NULL,grid_execute_thread,(void *)func_name);
	}
	void *grid_thread(void *data){

	};
	newera_hpc::newera_hpc(){
		pthread_mutex_init(&mutex,NULL);
		tasks = new grid_task;
	}
	newera_hpc::~newera_hpc(){
		delete tasks;
	}
	void newera_hpc::lock(){
		pthread_mutex_lock(&mutex);
	}
	void newera_hpc::unlock(){
		pthread_mutex_unlock(&mutex);
	}
	void newera_hpc::load(char *file_name){
		lock();
		void *dll = dlopen(file_name,RTLD_NOW);
		if(!dll){
			cout<<dlerror()<<endl;
			unlock();
			return;
		}
		func_ptr func_ptr_t = (func_ptr)dlsym(dll,"plugin_init");
		func_ptr func_ptr_t_c = (func_ptr)dlsym(dll,"plugin_exec_client");
		func_ptr func_ptr_t_p = (func_ptr)dlsym(dll,"plugin_processor");
		if(!func_ptr_t&&!func_ptr_t_c&&!func_ptr_t_p){
			cout<<dlerror()<<endl;
			unlock();
			return;
		}
		else{
			void *func_string_temp;
			func_string_temp = func_ptr_t(NULL);
			string *func_string = (string *)func_string_temp;
			if(check_dll((char*)(*func_string).c_str())==true)cout<<"dll already found"<<endl;
			if(check_dll((char*)(*func_string).c_str())==true){
				unlock();
				return;
			}
			func_ptr_t = (func_ptr)dlsym(dll,"plugin_exec");
			if(!func_ptr_t){
				unlock();
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
		unlock();
	}
	bool newera_hpc::check_dll(char *func_name){
		functions_map::iterator check = functions.find(func_name);
		if(check==functions.end())return false;
		else return true;
	}
	void *newera_hpc::execute(char *func_name){
		grid_task *task = NULL;
		if(check_dll(func_name)){
			void *tmp_retrn = functions[func_name]->ptr(NULL);
			task = (grid_task *)tmp_retrn;
			hpc_data->add_task(task);
		}
		else cout<<func_name<<" :module not found"<<endl;
		cout<<"task size is "<<task->size()<<endl;
		for(int cntr=0;cntr<task->size();cntr++){
			instruction_set *instruction = task->return_instruction();
			instruction->func_name = func_name;
			instruction->status = GRID_NDONE;
			peer_details *details = hpc_data->return_peer();
			while(details==NULL){
				cout<<"no free client found waiting for some time"<<endl;
				sleep(4);
				details = hpc_data->return_peer();
			}
			instruction->host = (char *)details->host.c_str();
			instruction->port = details->port;
			instruction->task = task;
			pthread_t thread;
			pthread_create(&thread,NULL,newera_hpc::send_job,(void *)instruction);
			details = NULL;
		}
		while(task->check()==false){
			sleep(2);
			cout<<"waiting for tasks to get over"<<endl;
		}
		functions[func_name]->ptr_processor(task->instructions);
		cout<<"the task completed successfuly"<<endl;
		delete task;
	}
	void *newera_hpc::send_job(void *data){
		instruction_set *instruction = (instruction_set *)data;
		conn_rec *out_rec = new conn_rec;
		out_rec->host = instruction->host;
		out_rec->port = instruction->port;
		connect(out_rec);
		if(!out_rec->conn_status)return data;
		instruction->id = database->add(out_rec->host,instruction->thread,instruction);
		network_write *write = new network_write(out_rec->sockfd);
		write->add("GRID_EXECUTE");
		write->add(instruction->func_name);
		write->add(itoa(server_port));
		write->add(itoa(instruction->id));
		write->add("");
		write->push();
		write->push_raw((char *)instruction->data,instruction->length);
		shutdown(out_rec->sockfd,SHUT_RDWR);
		close(out_rec->sockfd);
		while(1){
			if(instruction->status==GRID_DONE)break;
			else sleep(2);
		}
	}
	char *newera_hpc::return_path(char *plugin){
		return functions[plugin]->path;
	}
	void newera_hpc::execute_client(instruction_set *instruction){
		functions[instruction->plugin]->ptr_client(instruction);
		conn_rec *out_rec = new conn_rec;
		out_rec->host = instruction->host;
		out_rec->port = instruction->port;
		connect(out_rec);
		if(!out_rec->conn_status){
			cout<<"connection failed"<<endl;
			return;
		}
		network_write *write = new network_write(out_rec->sockfd);
		write->add("GRID_REPLY");
		write->add(itoa(instruction->id));
		write->add("");
		write->push();
		write->push_raw((char *)instruction->data,instruction->length);
		shutdown(out_rec->sockfd,SHUT_RDWR);
		close(out_rec->sockfd);
	}
};
