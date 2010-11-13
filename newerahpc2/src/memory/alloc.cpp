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
	void *alloc(size_t in_size){
		void *data = malloc(in_size);
		(*mem_obj).add_mem(data,in_size);
		return data;
	}
	void dalloc(void *in_p){
		mem_obj->rem_mem(in_p);
	}
	void mem::add_mem(void *in_p,size_t in_size){
		mem_element *element = new mem_element;
		element->data = in_p;
		element->size = in_size;
		(*elements) += (void *)element;
	}
	void mem::rem_mem(void *in_p){
		(*elements) -= in_p;
		free(in_p);
	}
};