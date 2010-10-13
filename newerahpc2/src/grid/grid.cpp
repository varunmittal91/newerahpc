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
				conn_rec *out_rec = new conn_rec;
				out_rec->host = in_rec->host;
				out_rec->port = atoi(req->lines[2]);
				connect(out_rec);
				if(!out_rec->conn_status)cout<<"connection failed"<<endl;
				else{
					network_write *write = new network_write(out_rec->sockfd);
					client_request *req_plugin = new client_request;
					req_plugin->get_file = GET_FILE;
					write->add("GRID_GETPLUGIN");
					write->add(req->lines[1]);
					write->add(itoa(server_port));
					write->add("");
					write->push();
					shutdown(out_rec->sockfd,SHUT_RDWR);
					close(out_rec->sockfd);
				}
			}
			for(int cntr_1=0;cntr_1<5;cntr_1++){
				sleep(2);
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
			hpc_data->load(req->file_location);
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
				length += itoa(get_file_size(hpc_data->return_path(req->lines[1])));
				network_write *write = new network_write(out_rec->sockfd);
				write->file_assign(hpc_data->return_path(req->lines[1]));
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
