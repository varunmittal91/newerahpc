//Copyright 2009 Varun Mittal ©
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

#include <init.h>
#include <stdio.h>
#include <string.h>

using namespace newera_network;

namespace newera_mpi{
	newera_hpc_data* init_data(){
     		newera_hpc_data *data = new newera_hpc_data;
     		data->digits_used = 0;
     		zero(data);
     		data->digits_used = digits_mpi-1;
     		data->count = digits_mpi;
     		return data;
  	}
  	void delete_data(newera_hpc_data *data){
	  	delete data;
	  	data = 0;
  	}
 	void re_init_data(newera_hpc_data *data){
     		zero(data);
     		data->digits_used = digits_mpi-1;
  	}
  	void zero(newera_hpc_data *data){
		//memset(data->data,0,sizeof(newera_hpc_bit)*data->count);
     		for(int a=data->digits_used;a<data->count;a++)data->data[a]=0;
  	}
  	void zero_manual(newera_hpc_data *data){
		zero(data);
     		//for(int a=0;a<data->count;a++)data->data[a]=0;
  	}
}
