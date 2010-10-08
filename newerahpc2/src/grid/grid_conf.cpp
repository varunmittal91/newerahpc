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
	void load_node_list(){
		ifstream cfg(conf_file);
		string line;
		while(getline(cfg,line)){
			if(line.find("peer:")!=string::npos){
				size_t init = line.find("peer:");
				size_t init_port = line.find(":",init+1);
				size_t port_x = line.find(":",init_port+1);
				string server = line.substr(init_port+1,port_x-init_port-1);
				string port = line.substr(port_x+1,line.length()-init_port);
				init = line.find(":",port_x+1);
				string processors = line.substr(init+1,line.length()-init);
				hpc_data->add_peer(server.c_str(),atoi(port.c_str()),atoi(processors.c_str()));
			}
		}
	}
	config::config(){
		thread = new pthread_mutex_t;	
	}
	config::~config(){
		
	}
	void config::lock_config(){
		pthread_mutex_lock(thread);
	}
	void config::unlock_config(){
		pthread_mutex_unlock(thread);
	}
	void config::add_peer(const char *host,int port,int processors){
		lock_config();
		peer_details *details = new peer_details;
		details->port = port;
		details->processors = processors;
		details->host = host;
		details->status = 0;
		peers_t[host] = details;
		unlock_config();
	}
	peer_details *config::return_peer(){
		lock_config();
		peer_details *details = NULL;
		peers::iterator loop;
		for(loop=peers_t.begin();loop!=peers_t.end();loop++){
			details = loop->second;
			if(details->status<details->processors){
				details->status++;
				break;
			}
			details = NULL;
		}
		unlock_config();
		return details;
	}
	void config::free_peer(string host){
		lock_config();
		peer_details *details = peers_t[host];
		details->status--;
		unlock_config();
	}
}
