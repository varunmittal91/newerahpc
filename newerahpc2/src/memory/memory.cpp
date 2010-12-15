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

using namespace std;

namespace newera_network{
	mem *mem_obj;
	int mem_obj_status;
	mem::mem(){
		cout<<"constructor called"<<endl;
		elements = new queue;
		mutex = new pthread_mutex_t;
		mem_obj_status = REGISTER;
	}
	mem::~mem(){
		pthread_mutex_lock(mutex);
		int cntr=0;
		for(int cntr=(elements->count-1);cntr>=0;cntr--){
			mem_element *tmp_elem = (mem_element *)(*elements)[cntr];
			free(tmp_elem->data);
			(*elements) -= cntr;
			delete tmp_elem;
		}
		pthread_mutex_unlock(mutex);
		delete elements;
		delete mutex;
	}
	void mem::lock(){
		pthread_mutex_lock(mutex);
	}
	void mem::unlock(){
		pthread_mutex_unlock(mutex);
	}
};