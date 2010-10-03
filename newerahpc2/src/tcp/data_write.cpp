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
	void network_write::file_assign(char *file_name){
		fp = fopen(file_name,"r");
	}
	network_write::network_write(int sockfd_out){
		sockfd = sockfd_out;
		fp = 0;
		lines = 0;
		lengths = NULL;
		buffer = NULL;
	}
	network_write::~network_write(){
		if(buffer!=NULL)
			delete []buffer;
		if(lengths!=NULL)
			delete []lengths;
	}
	void network_write::add(const char * str_in){
		if(lines==0){
			buffer = new char* [1];
			lengths = new int [1];
		}
		else{
			int *lengths_temp = new int [lines+1];
			char **line_temp = new char* [lines+1];
			memcpy(line_temp,buffer,sizeof(buffer)*lines);
			memcpy(lengths_temp,lengths,sizeof(lengths)*lines);
			delete []buffer;
			delete []lengths;
			buffer = line_temp;
			lengths = lengths_temp;
		}
		lengths[lines] = strlen(str_in);
		buffer[lines] = (char *)str_in;
		lines++;
	}
	void network_write::add(string *in){
		network_write::add((*in).data());
		lengths[lines-1] = (*in).length();
	}
	void network_write::push_raw(char *in,int length){
		write(sockfd,in,length);
	}
	int network_write::push(){
		size_t bytes = 0;
		size_t bytes_tmp = 0;
		for(int cntr_1=0;cntr_1<lines;cntr_1++){
			bytes_tmp = send(sockfd,buffer[cntr_1],lengths[cntr_1],0);
			bytes_tmp = send(sockfd,"\r",1,0);
			if(cntr_1+1!=lines){
				bytes_tmp += send(sockfd,"\n",1,0);
			}
			if(bytes_tmp<1)break;
			bytes += bytes_tmp;
		}
		size_t bytes_file = push_file(); 
		bytes += bytes_file;
		return bytes;
	}
	int network_write::push_http(){
		size_t bytes = 0;
		size_t bytes_tmp = 0;
		for(int cntr_1=0;cntr_1<lines;cntr_1++){
			bytes_tmp = send(sockfd,buffer[cntr_1],lengths[cntr_1],0);
			bytes_tmp = send(sockfd,"\n",1,0);
			if(bytes_tmp<1)break;
			bytes += bytes_tmp;
		}
		size_t bytes_file = push_file(); 
		bytes += bytes_file;
		return bytes;
	}
	int network_write::push_file(){
		if(fp==NULL){
			return 0;
		}
		write(sockfd,"\n",1);
		int bytes_temp = 0;
		size_t bytes_file;
		size_t bytes = 0;
		char buffer[BUFFSIZE];
		while(1){
			bzero(buffer,sizeof(buffer));
			bytes_file = fread(buffer,1,sizeof(buffer),fp);
			if(bytes_file==0)break;
			bytes_temp = write(sockfd,buffer,sizeof(buffer));
			if(bytes_temp<0){
				break;
			}
			bytes += bytes_temp;
		}
		fclose(fp);
		return bytes;
	}
	void network_write::reset(){
		fp = 0;
		lines = 0;
		delete []lengths;
		delete []buffer;
	}
}
