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
#include <unistd.h>
#include <errno.h>

using namespace std;

namespace newera_network{
	client_request::client_request(){
		lines = new char* [CLIENT_REQ_MAX];
		length = new int [CLIENT_REQ_MAX];
		limit = CLIENT_REQ_MAX;
		count = 0;
		file_size = 0;
		file_size_act = 0;
		get_file = NO_FILE;
	}
	client_request::~client_request(){
		for(int cntr=0;cntr<count;cntr++){
			delete lines[cntr];
		}
		delete lines;
		delete length;
	}
	void client_request::increase_limit(){
		char **temp_lines = new char* [CLIENT_REQ_MAX+limit];
		memcpy(temp_lines,lines,sizeof(char *)*limit);
		delete lines;
		lines = temp_lines;
		int *temp_length = new int [CLIENT_REQ_MAX+limit];
		memcpy(temp_length,length,sizeof(int)*limit);
		delete length;
		length = temp_length;
		limit = limit+CLIENT_REQ_MAX;
	}
	void client_request::read_file(conn_rec *in_rec,char *temp,int remain){
		fp.write(temp,remain);
		file_size += remain;
		int bytes;
		char buffer[BUFFSIZE];
		do{
			bzero(buffer,sizeof(buffer));
			bytes = recv(in_rec->sockfd,buffer,sizeof(buffer),0);
			fp.write(buffer,bytes);
			file_size += bytes;
			if(file_size_act!=0){
				if(file_size==file_size_act)break;
				else if(bytes==0)break;
			}
		}while(bytes>=0);
		fp.close();
	}
	void client_request::read(conn_rec *in_rec){
		signal(SIGPIPE,sig_handler);
		int bytes = 0;
		char buffer[BUFFSIZE];
		int flag = 0;
		string file = return_file();
		if(get_file==GET_FILE){
			file_location = (char *)file.c_str();
			fp.open(file_location.c_str(),ios::binary);
		}
		do{
			bzero(buffer,sizeof(buffer));
			bytes = recv(in_rec->sockfd,buffer,sizeof(buffer),0);
			if(bytes<0)
				perror("read faile: ");
			char *temp_buffer = buffer;
			int postn = 0;
			for(int cnt_1=0;cnt_1<bytes;cnt_1++){
				if((count)==CLIENT_REQ_MAX){
					increase_limit();
				}
				if(buffer[cnt_1]=='\r'){
					int str_size = cnt_1-postn;
					if(str_size==0){
						flag = 1;
						temp_buffer++;
						cnt_1++;
						if(get_file==GET_FILE){
							temp_buffer++;
							cnt_1++;
							read_file(in_rec,temp_buffer,bytes-cnt_1);
						}
						else if(get_file==GET_GRID){
							grid_data *grid_data_t = new grid_data;
							left_over = new char [bytes-cnt_1];
							memcpy(left_over,temp_buffer,bytes-cnt_1);
							grid_data_t->add(left_over,(size_t)(bytes-cnt_1));
							while(1){
								bzero(buffer,sizeof(buffer));
								bytes = recv(in_rec->sockfd,buffer,sizeof(buffer),0);
								if(bytes==0)break;
								grid_data_t->add(buffer,(size_t)bytes);
							}
							grid_data_v = grid_data_t->data_return();
							delete grid_data_t;
						}
						break;
					}
					lines[count] = new char [str_size+1];
					bzero(lines[count],sizeof(lines[count]));
					strncpy(lines[count],temp_buffer,str_size);
					lines[count][str_size] = '\0';
					length[count] = str_size;
					temp_buffer = &(buffer[cnt_1+1]);
					if(find(lines[count],(char *)"Length")!=STR_NPOS){
						size_t pos_tmp = find(lines[count],(char *)": ");
						if(pos_tmp!=STR_NPOS){
							char *xyz = substr(lines[count],pos_tmp+2,strlen(lines[count]));
							file_size_act = atoi(xyz);
							delete xyz;
							get_file = GET_FILE;
							file_location = file;
							fp.open(file_location.c_str(),ios::binary);
						}
						else get_file = NO_FILE;
					}
					else if((find(lines[count],(char *)"GRID_EXECUTE")!=STR_NPOS)||(find(lines[count],(char *)"GRID_REPLY"))!=STR_NPOS){
						get_file = GET_GRID;
					}
					count++;
					postn = cnt_1+1;
				}
				else if(buffer[cnt_1]=='\n'){
					temp_buffer++;
					postn++;
				}
			}
			if(flag)break;
		}while(bytes==-1 && errno==EINTR);
	}
};
