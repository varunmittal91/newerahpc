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
	char *http_fields[4] = {(char *)"Host:",(char *)"User-Agent:",(char *)"Accept:",(char *)"Accept-Language:"};
	int http_fields_int = 4;
	tcp_http_response_codes http_response_codes[16] = {
	200,(char *)"OK",                         //0
	201,(char *)"Created",                    //1
	202,(char *)"Accepted",                   //2
	204,(char *)"No Content",                 //3
	300,(char *)"Multiple",                   //4
	301,(char *)"Moved Permanently",          //5
	302,(char *)"Moved Temporarily",          //6
	304,(char *)"Not Modified",               //7
	400,(char *)"Bad Request",                //8
	401,(char *)"Unauthorized",               //9
	403,(char *)"Forbidden",                  //10
	404,(char *)"File Not Found",             //11
	500,(char *)"Internal Server Error",      //12
	501,(char *)"Not Implemented",            //13
	502,(char *)"Bad Gateway",                //14
	503,(char *)"Service"                     //15
	};
	http_data::http_data(conn_rec *in_rec,client_request *req){
		header = new http_server_header;
		header->lines = new string *[HTTP_SERVER_HEADER_MAX];
		header->count = 0;
		req_details = new char* [REQ_DETAILS_MAX];
		status_code = 0;
		file_status = 0;		
		lng_supp = false;
		sockfd = in_rec->sockfd;
		write = new network_write(sockfd);
		for(int cntr_1=0;cntr_1<req->count;cntr_1++){
			size_t pos_1,pos_2,pos_3;
			if((pos_1 = find(req->lines[cntr_1],(char *)"GET"))!=STR_NPOS){
				pos_2 = find(req->lines[cntr_1],(char *)"/");
				pos_3 = find(req->lines[cntr_1],(char *)" ",pos_2+1);
				req_details[0] = substr(req->lines[cntr_1],pos_2,pos_3);	//requested file
				size_t length = strlen(req_details[0])+1;
				req_details[FILE_REQUEST_ORIG] = (char *)malloc(length);
				bzero(req_details[FILE_REQUEST_ORIG],length);
				strcpy(req_details[FILE_REQUEST_ORIG],req_details[FILE_REQUEST]);
				req_details[FILE_REQUEST_ORIG-1]='\0';
				pos_1 = find(req->lines[cntr_1],(char *)"HTTP");
				pos_2 = find(req->lines[cntr_1],(char *)"/",pos_1);
				pos_2++;
				req_details[1] = substr(req->lines[cntr_1],pos_2,req->length[cntr_1]); //http version
			}
			else{
				for(int cntr_2=0;cntr_2<http_fields_int;cntr_2++){
					if((pos_1 = find(req->lines[cntr_1],http_fields[cntr_2]))!=STR_NPOS){
						pos_2 = find(req->lines[cntr_1],(char *)":");
						pos_2 = pos_2 + 2;
						req_details[cntr_2+2]=substr(req->lines[cntr_1],pos_2,req->length[cntr_1]);
					}
				}
			}
		}
	}
	http_data::~http_data(){
                delete write;
                delete []header->lines;
                delete header;
	}
	void http_init(conn_rec *in_rec,client_request *req){
		http_data *client_http_data = new http_data(in_rec,req);
		client_http_data->file_request();
		client_http_data->gen_header();
		client_http_data->reply();
		delete client_http_data;
	}
};
