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
#include <iostream>

using namespace std;

namespace newera_network{
	char *itoa(int num){
  		int temp_num = num;
  		int counter = 0;
  		while(temp_num>0){
    			counter++;
    			temp_num = temp_num/10;
  		}
 		char *buffer = new char [counter+1];
  		buffer[counter] = '\0';
 	 	while(num>0){
    			temp_num = num%10;
    			buffer[counter-1] = (char)(temp_num+48);
    			num = num/10;
    			counter--;
	  	}              
  		return buffer;
	}
	size_t find(char *str1,char *str2){
		if(strlen(str2)>strlen(str1))return STR_NPOS;
		char *t_str1 = str1;
		char *t_str2 = str2;
		int flag = 0;
		size_t pos = 0;
		int count = 0;
		while(*t_str1!='\0'){
			if(*t_str2=='\0')break;
			if(*t_str1==*t_str2){
				if(flag==0)pos = count;
				flag=1;
				t_str2++;
			}
			else{
				if(flag==1){
					flag=0;
					t_str2 = str2;
				}
			}
			t_str1++;
			count++;
		}
		if(flag==1)return pos;
		else return STR_NPOS;
	}
	size_t find(char *str1,char *str2,size_t next){
		char *t_str1 = str1;
		char *t_str2 = str2;
		if((strlen(str1)-1)<next)return STR_NPOS;
		for(int cntr_1=0;cntr_1<next;cntr_1++)t_str1++;
		if(strlen(str2)>strlen(str1))return STR_NPOS;
		int flag = 0;
		size_t pos = 0;
		int count = 0;
		while(*t_str1!='\0'){
			if(*t_str2=='\0')break;
			if(*t_str1==*t_str2){
				if(flag==0)pos = count;
				flag=1;
				t_str2++;
			}
			else{
				if(flag==1){
					flag=0;
					t_str2 = str2;
				}
			}
			t_str1++;
			count++;
		}
		pos+=next;
		if(flag==1)return pos;
		else return STR_NPOS;
	}
	char *substr(char *str,size_t start_pos,size_t end_pos){
		size_t length = end_pos - start_pos;
		char *result = new char [length+1];
		bzero(result,sizeof(result));
		char *temp = str;
		for(int cntr_1=0;cntr_1<start_pos;cntr_1++){
			temp++;
		}
		strncpy(result,temp,length);
		result[length] = '\0';
		return result;
	}
};

