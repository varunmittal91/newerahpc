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
	newera_hpc *hpc_data;
	config *newerahpc_config;
	
	void grid_init(conn_rec *in_rec,client_request *req){
		if(find(req->lines[0],(char *)"_EXECUTE")!=STR_NPOS){
			if(!hpc_data->check_dll(req->lines[1])){
				hpc_data->add_request((char *)in_rec->host,atoi(req->lines[2]),(char *)req->lines[1],(char *)"",WAIT_PLUGIN);
			}
			for(int cntr_1=0;cntr_1<5;cntr_1++){
				sleep(4);
				if(hpc_data->check_dll(req->lines[1])){
					instruction_set *instruction = new instruction_set;
					instruction->host = in_rec->host;
					instruction->port = atoi(req->lines[2]);
					instruction->plugin = req->lines[1];
					instruction->data = req->grid_data_v;
					instruction->id = atoi(req->lines[3]);
					hpc_data->execute_client(instruction);
					free(req->grid_data_v);
					delete instruction;
					break;
				}
			}			
		}
		else if(find(req->lines[0],(char *)"_LOADPLUGIN")!=STR_NPOS){
			hpc_data->load((char *)req->file_location.c_str());
		}
		else if(find(req->lines[0],(char *)"_REPLY")!=STR_NPOS){
			int id = atoi(req->lines[1]);
			conn_details::link *link = database->return_details(in_rec->host,id);
			instruction_set *instruction = (instruction_set *)link->data_univ;
			instruction->status = GRID_DONE;
			instruction->data = req->grid_data_v;
			hpc_data->free_peer(in_rec->host);
		}
		else if(find(req->lines[0],(char *)"_GETPLUGIN")!=STR_NPOS){
			if(hpc_data->check_dll(req->lines[1])){
				conn_rec *out_rec = new conn_rec;
				out_rec->host = in_rec->host;
				out_rec->port = atoi(req->lines[2]);
				connect(out_rec);
				string length = "Content-Length: ";
				char *path = (char *)hpc_data->return_path(req->lines[1]).c_str();
				string str = hpc_data->return_path(req->lines[1]);
				cout<<str<<endl;
				length += itoa(get_file_size(path));
				network_write *write = new network_write(out_rec->sockfd);
				write->file_assign(path);
				cout<<"sending hpc_data->plugin "<<path<<endl;
				write->add("GRID_LOADPLUGIN");
				write->add(length.c_str());
				write->add("");
				write->push();
				sleep(2);
				shutdown(out_rec->sockfd,SHUT_RDWR);
				close(out_rec->sockfd);
				delete out_rec;
				delete write;
			}
		}
	}
};
