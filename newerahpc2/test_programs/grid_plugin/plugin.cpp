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

#include <string>
#include <iostream>
#include "plugin.h"
#include <init.h>

using namespace std;
using namespace newera_network;
using namespace newera_mpi;

void print_num(newera_hpc_bit *num,int count){
        cout<<num[0]<<" "<<endl;
        int b =0;
        for(int a=1;a<count;a++){
                cout<<num[a];
                if(a%10==0){
                        cout<<" ";
                        b++;
                        if(b==5){
                                cout<<endl;
                                b=0;
                        }
                }
        }
        cout<<endl;
}

extern "C"{
	string *plugin_init(mem *mem_obj_hst){
		mem_obj = mem_obj_hst;
		cout<<"function loaded successfully"<<endl;
		string *abcx = new string;
		(*abcx) = "newera_task";
		return abcx;
	}
	grid_task *plugin_exec(){
		int *a = new int;
		*a = 12;
		int *b = new int;
		*b = 34;
		grid_task *task = new grid_task;
		instruction_set *in1 = new instruction_set;
		in1->data = (void *)a;
		in1->length = sizeof(int);
		instruction_set *in2 = new instruction_set;
		in2->data = (void *)b;
		in2->length = sizeof(int);
		(*task) = in1;
		(*task) = in2;
		return task;
	}
	instruction_set *plugin_exec_client(void *data){
		cout<<"executing client function"<<endl;
		instruction_set *instruction = (instruction_set *)data;
		int *values = (int *)instruction->data;
		newera_hpc_data *data_mpi = arctan(values[0],values[2],values[1]);
		instruction->data = (void *)data_mpi->data;
		instruction->length = sizeof(newera_hpc_bit)*4000;
		return instruction;
	}
	void *plugin_processor(void *data){
		instruction_set **instructions = (instruction_set **)data;
		instruction_set *in1,*in2,*in3;
		in1 = instructions[0];
		in2 = instructions[1];
		in3 = instructions[2];
		newera_hpc_bit *a = (newera_hpc_bit *)in1->data;
		newera_hpc_bit *b = (newera_hpc_bit *)in2->data;
		newera_hpc_bit *c = (newera_hpc_bit *)in3->data;
		multiply_old(a,16,4000);
		add_old(b,c,4000);
		multiply_old(b,4,4000);
		subtract_old(a,b,4000);
		print_num(a,4000);
		print_num(b,4000);
		cout<<"i have all the data from clients"<<endl;
	}
}
