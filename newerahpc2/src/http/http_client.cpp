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
	http_data_client::http_data_client(){
		url = new string;
		host = new string;
		file_request = new string;
		protocol = new string;
		out_rec = new conn_rec;
	}
	http_data_client::~http_data_client(){
		delete host;
		delete url;
		delete protocol;
		delete file_request;
		delete write;
		delete out_rec;
	}
	void http_data_client::operator=(char *url_char){
		(*url) = url_char;
		arrange();
		out_rec = new conn_rec;
		out_rec->host = (char *)(*host).c_str();
		out_rec->port = port;
		connect(out_rec);
		if(!(out_rec->conn_status))cout<<"connection failed"<<endl;
		write = new network_write(out_rec->sockfd);
	}
	void http_data_client::arrange(){
  		size_t pos_1 = 0;
  		size_t pos_2 = 0;
  		pos_1 = (*url).find("://");
  		if(pos_1!=string::npos){
        		(*protocol) = (*url).substr(0,pos_1);
        		pos_1 += 3;
  		}
  		else{
        		(*protocol) = "http";
        		pos_1 = 0;
  		}
  		pos_2 = (*url).find("/",pos_1);
  		if(pos_2!=string::npos){
        		(*file_request) = (*url).substr(pos_2,(*url).length()-pos_2);
  		}
  		else{
        		(*file_request) = "/";
		}
  		(*host) = (*url).substr(pos_1,pos_2-pos_1);
		if((*host).find(":")!=string::npos){
			size_t pos_t = (*host).find(":");
			string post_str = (*host).substr(pos_t+1,(*host).length()-pos_t-1);
			port = atoi(post_str.c_str());
			(*host) = (*host).substr(0,pos_t);
		}
		else{
			if((*protocol)=="http")port = 80;
			else if((*protocol)=="https")port = 443;
		}
	}
	void http_data_client::display(){
		cout<<"host: "<<(*host)<<endl;
		cout<<"protocol: "<<(*protocol)<<endl;
		cout<<"file_request: "<<(*file_request)<<endl;
	}
	void http_data_client::push_req(){
		string temp;
		temp = "GET ";
		temp += (*file_request);
		temp += " HTTP/1.1";
		write->add(temp.c_str());
		write->push();
		write->reset();
		temp = "Host: ";
		temp += (*host);
		write->add(temp.c_str());
		write->add("User-Agent: Newera");
		write->add("");
		write->push();
	}
};
