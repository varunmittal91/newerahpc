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
#include <string>

using namespace std;

namespace newera_network{
	void http_data::gen_header(){
		header->count = 0;
		string *line;
		header->lines[header->count] = new string;
		line = header->lines[header->count];
		(*line) = "HTTP/";
		(*line) += req_details[1];
		(*line) += " ";
		(*line) += itoa(http_response_codes[status_code].code);
		(*line) += " ";
		(*line) += http_response_codes[status_code].message;
		header->count++;
		header->lines[header->count] = new string;
		line = header->lines[header->count];
		(*line) = "Server: ";
		(*line) += SERVER_VER;
		if(file_status==FILE_FOUND&&lng_supp==false){
			header->count++;
			header->lines[header->count] = new string;
			line = header->lines[header->count];
			(*line) = "Content-Length: ";
			(*line) += itoa(file_size);
		}
		if(lng_supp==true){
			header->count++;
			header->lines[header->count] = new string;
			line = header->lines[header->count];
			(*line) = "Content-Type: text/plain";
		}
		header->count++;
	}
	void http_data::reply(){
		if(file_status==FILE_FOUND&&lng_supp!=true){
			write->file_assign(req_details[FILE_REQUEST]);
		}
		for(int cntr_1=0;cntr_1<header->count;cntr_1++){
			write->add(header->lines[cntr_1]->data());
		}
		if(lng_supp==true)hpc_data->grid_lng_init(write,req_details[FILE_REQUEST]);
		write->push_http();
		push_header();
	}
	void http_data::directory_listing(){
		if(file_status!=DIR_FOUND)return;
		struct dirent *ep;
		DIR *dp = opendir(req_details[FILE_REQUEST]);
		if(dp==NULL)return;
		write->add("<table width=\"100%\" ><tr><td>Name</td><td>Type</td><td>Size</td></tr>");
		while(ep = readdir(dp)){
			string name = ep->d_name;
			if(name[0]=='.')continue;
			int file_check = filedir_check(req_details[FILE_REQUEST]);
			string linx = req_details[FILE_REQUEST_ORIG];
			if(linx[linx.length()-1]=='/')
				linx += name;
			else
				linx += "/" + name;
			write->add("<tr><td>");
			write->add("<a href='");
			write->add(linx.data());
			write->add("'>");
			write->add(name.data());
			write->add("</a></td><td>");
			if(file_check==FILE_FOUND)
				write->add("FILE</td><td>");
			else
				write->add("Directory</td><td>");
			write->add("</td></tr>");
			write->push_http();
			write->reset();
		}
		write->add("</table>");
		closedir(dp);
	}
	void http_data::push_header(){
		if(file_status==FILE_FOUND)return;
		write->reset();
		write->add("\n");
		write->add("<html><Head><Title>");
		write->add(SITE_NAME);
		write->add("</Title></Head><Body><h1><center>newera server</center></h1><hr>");
		directory_listing();
		write->add("</Body></Html>");
		write->push_http();
	}
};
