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

void* operator new(std::size_t in_s) throw(std::bad_alloc){
	void *p = malloc(in_s);
	while(p==0){
		p = malloc(in_s);
	}
	if(newera_network::mem_obj_status==REGISTER){
		(*newera_network::mem_obj).add_mem(p,in_s);
	}
	return p;	
}
void* operator new(std::size_t in_s, const std::nothrow_t&) throw(){
	void *p = malloc(in_s);
	while(p==0){
		p = malloc(in_s);
	}
	if(newera_network::mem_obj_status==REGISTER){
		(*newera_network::mem_obj).add_mem(p,in_s);
	}
	return p;	
}
void* operator new[](std::size_t in_s) throw(std::bad_alloc){
	return ::operator new(in_s);
}
void* operator new[](std::size_t in_s, const std::nothrow_t&) throw(){
	return ::operator new(in_s);
}

void operator delete(void *in_p){
	newera_network::mem_obj->rem_mem(in_p);
};   
void operator delete[](void *in_p){
	::operator delete(in_p);
};
 
namespace newera_network{
	void *expand_mem(void *in_p,size_t in_size){
		
	}
	void *mem::expand_mem(void *in_p,size_t in_size){
		
	}
	void mem::rem_mem_clean(void *in_p){
		mem_element *tmp_elem = (mem_element *)locate(in_p);
		if(tmp_elem==NULL)return;
		lock();
		(*elements) -= tmp_elem;
		unlock();
		free(tmp_elem->data);
		delete tmp_elem;
	}
	void mem::rem_mem(void *in_p){
		mem_element *tmp_elem = (mem_element *)locate(in_p);
		if(tmp_elem==NULL){
			return;
		}
		lock();
		(*elements) -= tmp_elem;
		unlock();
		delete tmp_elem;
	}		
	void *mem::locate(void *in_p){
		lock();
		for(int cntr=0;cntr<elements->count;cntr++){
			mem_element *tmp_elem = (mem_element *)(*elements)[cntr]; 
			if(in_p==tmp_elem->data)return tmp_elem;
		}
		return NULL;
		unlock();
	}
	void mem::add_mem(void *in_p,size_t in_size){
		mem_element *element = (mem_element *)malloc(sizeof(mem_element));
		element->data = in_p;
		element->size = in_size;
		lock();
		(*elements) += (void *)element;
		unlock();
	}	
};
