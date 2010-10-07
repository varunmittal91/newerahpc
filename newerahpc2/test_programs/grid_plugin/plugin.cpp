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

using namespace std;
using namespace newera_network;

extern "C"{
	string *plugin_init(){
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
		cout<<"yup i m borm now"<<endl;
		instruction_set *instruction = (instruction_set *)data;
		int *a = (int *)instruction->data;
		*a = (*a) * 2;
		cout<<*a<<" in exec_client"<<endl;
		instruction->data = (void *)a;
		instruction->length = sizeof(int);
		return instruction;
	}
	void *plugin_processor(void *data){
		instruction_set **instructions = (instruction_set **)data;
		instruction_set *in1;
		in1 = instructions[0];
		int *a = (int *)in1->data;
		cout<<"this my data "<<*a<<endl;
		cout<<"i have all the data from clients"<<endl;
	}
}
