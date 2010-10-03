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
	conn_database::conn_database(){
		pthread_mutex_init(&mutex,NULL);
		plugin_manager = new plugin_manager_t;
		plugin_manager->count = 0;
	}
	conn_database::~conn_database(){
		delete plugin_manager;
	}
	void conn_database::display(char *client_ip){
		cout<<"num of connections from "<<client_ip<<" ";
		cout<<clients[client_ip]->count<<endl;
		conn_details *details = clients[client_ip];
		conn_details::link *link_t = details->next_link;
		for(int cntr=0;cntr<details->count;cntr++){
			cout<<" ->"<<link_t->cid<<endl;
			link_t = link_t->next_link;
		}
	}
	conn_details::link *conn_database::return_details(char *client_ip,int cid){
		conn_details *details = clients[client_ip];
		conn_details::link *link_t = details->next_link;
		for(int cntr=0;cntr<details->count;cntr++){
			if(link_t->cid==cid)break;
			link_t = link_t->next_link;
		}
		return link_t;
	}
	void conn_database::remove(char *client_ip,int cid){
		conn_details *details = clients[client_ip];
		conn_details::link *link_t1 = details->next_link;
		conn_details::link *link_t2,*link_p;
		for(int cntr=0;cntr<details->count;cntr++){
			if(link_t1->cid==cid){
				link_t2 = link_t1->next_link;
				delete link_t1;
				if(cntr==0)
					details->next_link = link_t2;
				else
					link_p->next_link = link_t2;
				details->count--;
				break;
			}
			link_p = link_t1;
			link_t1 = link_t1->next_link;
		}
	}
	int conn_database::add(char *client_ip,pthread_t thread){
		pthread_mutex_lock(&mutex);
		clients_t::iterator it = clients.find(client_ip);
		conn_details::link *link_n = new conn_details::link;
		link_n->next_link = NULL;
		link_n->thread = thread;
		if(it!=clients.end()){
			conn_details *details = clients[client_ip];
			conn_details::link *link_t = details->next_link;
			details->count++;
			details->curr_cid++;
			link_n->cid = details->curr_cid;
			if(link_t!=NULL){
				while(link_t->next_link!=NULL){
					link_t = link_t->next_link;
				}
				link_t->next_link = link_n;
			}
			else{
				details->next_link = link_n;
			}
		}
		else{
			conn_details *details = new conn_details;
			details->count = 1;
			details->curr_cid = 1;
			details->next_link = link_n;
			link_n->cid = 1;
			clients[client_ip] = details;
		}
		pthread_mutex_unlock(&mutex);
		return link_n->cid;
	}
	int conn_database::add(char *client_ip,pthread_t thread,void *data){
		int id = add(client_ip,thread);
		conn_details::link *link = return_details(client_ip,id);
		link->data_univ = data;
		return id;
	}
};
