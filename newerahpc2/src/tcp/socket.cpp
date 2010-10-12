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

using namespace std;

namespace newera_network{
	conn_rec *local_rec;
	conn_database *database;
	int server_port;

	void connection_close(conn_rec *in_rec){
		shutdown(in_rec->sockfd,SHUT_RDWR);
		close(in_rec->sockfd);
		delete in_rec;
	}
	void *handle_connection(void *data){
		signal(SIGPIPE,sig_handler);
		signal(SIGSEGV,sig_handler);
		conn_rec *in_rec = (conn_rec *)data;
		client_request *req = new client_request;
		req->read(in_rec);
		if(req->count>0){
			if(find(req->lines[0],(char *)"HTTP/1.")!=STR_NPOS){
				http_init(in_rec,req);
			}
			else if(find(req->lines[0],(char *)"GRID_")!=STR_NPOS){
				grid_init(in_rec,req);
			}
		}
		delete req;
		database->remove(in_rec->host,in_rec->cid);
		connection_close(in_rec);
	}
	void start(){
		signal(SIGINT,sig_handler);
		signal(SIGKILL,sig_handler);
		database = new conn_database;
		hpc_data = new newera_hpc;
		load_node_list();
		cout<<"enter a port for server communication: ";
		cin>>server_port;
		pthread_t main_thread;
		pthread_create(&main_thread,NULL,open_socket,NULL);
	}
	void *open_socket(void *){
		local_rec = new conn_rec;
		local_rec->host = (char *)"localhost";
		local_rec->addr.sin_family = AF_INET;
		local_rec->addr.sin_port = htons(server_port);
		local_rec->addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if((local_rec->sockfd = socket(PF_INET,SOCK_STREAM,0))<0)perror("");
		if(bind(local_rec->sockfd,(struct sockaddr *)&(local_rec->addr),sizeof(local_rec->addr))<0){
			perror("");
			exit(1);
		}
		if(listen(local_rec->sockfd,MAXPENDING)<0){
			perror("");
			exit(1);
		}
		while(1){
			conn_rec *client_rec = new conn_rec;
			int size = sizeof(client_rec->addr);
			int conn_client;
			if((client_rec->sockfd = accept(local_rec->sockfd,(struct sockaddr *)&(client_rec->addr),(socklen_t *)&(size)))<0){
				perror("");
				delete client_rec;
				continue;
			}
			pthread_t client_connect;
			client_rec->host = inet_ntoa(client_rec->addr.sin_addr);
			client_rec->port = ntohs(client_rec->addr.sin_port);
			client_rec->cid = database->add(client_rec->host,client_connect);
			client_rec->servr_port = server_port;
			int thread_accept_status = pthread_create(&client_connect,NULL,handle_connection,(void *)client_rec);
		}
	}
	void connect(conn_rec *in){
		in->addr.sin_family = AF_INET;
		in->addr.sin_port = htons(in->port);
		in->host_info = gethostbyname(in->host);
		if(in->host_info==NULL){
			cout<<"host not found"<<endl;
			return;
		}
		bcopy((char *)in->host_info->h_addr,(char *)&in->addr.sin_addr.s_addr,in->host_info->h_length);		
		in->addr.sin_port = htons(in->port);
		int size = sizeof(in->addr);
		if((in->sockfd = socket(PF_INET,SOCK_STREAM,0))<0){
			in->conn_status = false;
			return;
		}
		if(connect(in->sockfd,(struct sockaddr*)&(in->addr),(socklen_t)(size))<0){
			in->conn_status = false;
			cout<<"connection_failed"<<endl;
			return;
		}
		in->conn_status = true;
	}
};
