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
	grid_data::grid_data(){
		length = 0;
		count = 0;
		data_final = NULL;
	}
	grid_data::~grid_data(){
		for(int cntr=0;cntr<count;cntr++)free(data[cntr]);
		delete []data;
		delete []lengths;
	}
	void grid_data::add(char *in_line,size_t in_length){
		expand();
		data[count-1] = (char *)malloc(in_length);
		memcpy(data[count-1],in_line,in_length);
		lengths[count-1] = in_length;
		length += in_length;
	}
	void grid_data::expand(){
		count++;
		char **new_data = new char* [count];
		size_t *new_lengths = new size_t [count];
		if(count!=1){
			memcpy(new_data,data,sizeof(char**)*(count-1));
			memcpy(new_lengths,lengths,sizeof(size_t)*(count-1));
			delete []data;
			delete []lengths;
		}
		data = new_data;
		lengths = new_lengths;
	}
	void grid_data::display(){
		cout<<"data fetched :"<<length<<endl;
		for(int cntr=0;cntr<count;cntr++){
			cout<<data[cntr]<<" "<<lengths[cntr]<<endl;
		}
	}
	void *grid_data::data_return(){
		int done = 0;
		data_final = malloc(length);
		char *data_temp = (char *)data_final;
		for(int cntr_1=0;cntr_1<count;cntr_1++){
			memcpy(data_temp,data[cntr_1],lengths[cntr_1]);
			if(cntr_1!=(count-1)){
				for(int cntr_2=0;cntr_2<lengths[cntr_1];cntr_2++)data_temp++;
			}
		}
		return data_final;
	}
}