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
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
	
using namespace std;

namespace newera_mpi{
	string itoa(int num){
		ostringstream os;
		os<<num;
		return os.str();
    	}
    	newera_hpc_data *str_newera_hpc(string *input){
        	int flag=0,a=0,b=0;
        	newera_hpc_data *data = init_data();
        	for(b=0;b<(*input).length();b++){
            		string temp = (*input).substr(b,1);
            		if(flag==0){
               			if(temp=="0")a++;
               			else flag = 1;
            		}
            		data->data[b] = atoi(temp.c_str());
        	}
        	data->digits_used = a;
        	return data;
    	}
    	string newera_hpc_str(newera_hpc_data *input){
		string data;
        	for(int a=0;a<input->count;a++){
	    		(data)+=itoa(input->data[a]);
        	}
        	return data;
    	}
	char* newera_hpc_char(newera_hpc_data *input){
		char *data = new char [digits_mpi];
		for(int a=0;a<digits_mpi;a++){
			data[a] = (int)(48 + input->data[a]);
		}
		return data;
	}
	newera_hpc_data* char_newera_hpc(char *data,int count){
		newera_hpc_data *conv = init_data();
		int flag = 0,digits_used = 0;
		for(int a=0;a<count;a++){
			if(flag==0){
				if(data[a]=='0')digits_used++;
				else flag = 1;
			}
			conv->data[a] = (int)(data[a]-48);
		}
		conv->digits_used = digits_used;
		return conv;
	}
};
