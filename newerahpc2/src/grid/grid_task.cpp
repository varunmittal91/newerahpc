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

namespace newera_network{
	grid_task::grid_task(){
		c_id = 0;
		count = 0;
		used = 0;
	}
	grid_task::~grid_task(){
		lock();
		for(int cntr=0;cntr<count;cntr++){
			remove(instructions[cntr]);
			delete instructions[cntr];
		}
		unlock();
	}
	void grid_task::lock(){
		pthread_mutex_lock(&mutex);
	}
	void grid_task::unlock(){
		pthread_mutex_unlock(&mutex);
	}
	void grid_task::operator=(instruction_set *instruction){
		lock();
		count++;
		c_id++;
		instruction->id = c_id;
		instruction->status = GRID_NDONE;
		instruction_set **new_instructions = new instruction_set* [count];
		if(count!=1){
			memcpy(new_instructions,instructions,sizeof(instruction_set**)*(count-1));
			delete []instructions;
		}
		instructions = new_instructions;
		instructions[count-1] = instruction;
		unlock();
	}
	instruction_set *grid_task::return_instruction(){
		lock();
		int pos = count-used-1;
		instruction_set *instruction = instructions[pos];
		used++;
		unlock();
		return instruction;
	}
	void grid_task::remove(instruction_set *instruction){
		database->remove(instruction->host,instruction->id);
	}
	bool grid_task::check(){
		lock();
		instruction_set *instruction;
		bool ret_status = true;
		for(int cntr=0;cntr<count;cntr++){
			instruction = instructions[cntr];
			if(instruction->status==GRID_NDONE)ret_status = false;
		}
		unlock();
		return ret_status;
	}
	void grid_task::wait(){
		while(!check()){}
	}
	void grid_task::submit(){
		lock();
		for(int cntr=0;cntr<count;cntr++){
			instruction_set *instruction = return_instruction();
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
#ifdef debug
			add_log("task being dispatched");
			add_log((const char *)instruction->host);
#endif
			pthread_t thread;
			pthread_create(&thread,NULL,newera_hpc::send_job,(void *)instruction);
			details = NULL;
		}
		while(check()==false){
			sleep(2);
			cout<<"waiting for tasks to get over"<<endl;
		}
		hpc_data->functions[func_name]->ptr_processor(instructions);
		cout<<"the task completed successfuly"<<endl;		
		unlock();
	}
	task_manager::task_manager(){
		count = 0;
	}
	task_manager::~task_manager(){
	}
	int task_manager::add_task(grid_task *task){
		lock_task();
		count++;
		c_id++;
		grid_task **new_tasks = new grid_task* [count];
		task->c_id = c_id;
		if(count!=1){
			memcpy(new_tasks,tasks,sizeof(grid_task**)*(count-1));
			delete []tasks;
		}
		tasks = new_tasks;
		tasks[count-1] = task;
		unlock_task();
		return c_id;
	}
	void task_manager::remove_task(){
	}
	void task_manager::lock_task(){
		pthread_mutex_lock(&mutex);
	}
	void task_manager::unlock_task(){
		pthread_mutex_unlock(&mutex);
	}
};
