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

#define HTTP_SERVER_HEADER_MAX 10
#define REQ_DETAILS_MAX 7
#define FILE_DIR_NFOUND -1
#define FILE_FOUND 1
#define DIR_FOUND 2
#define SERVER_VER "newera server 1.0"
#define FILE_REQUEST 0
#define FILE_REQUEST_ORIG 6
#define SITE_NAME "newera server"


namespace newera_network{
	struct tcp_http_response_codes{
		int code;
		char *message;
   	};
	struct http_server_header{
		std::string **lines;
		int count;
	};
	class http_data{
		char *req_details[REQ_DETAILS_MAX];
		int file_status;
		int status_code;
		int sockfd;
		char *file_size_str;
		long file_size;
		http_server_header *header;
		network_write *write;
		bool lng_supp;
	public:
		http_data(conn_rec *,client_request *);
		~http_data();
		void file_request();
		void get_file_size();
		void gen_header();
		void reply();
		void push_header();
		void directory_listing();
	};
	class http_data_client{
		int port;
		std::string *host;
		std::string *file_request;
		std::string *protocol;
		std::string *url;
		client_request *request;
		void arrange();
	public:
		network_write *write;
		conn_rec *out_rec;
		http_data_client();
		~http_data_client();
		void operator=(char *);
		void display();
		void push_req();
		int conn_status(){return out_rec->conn_status;}
	};
	extern tcp_http_response_codes http_response_codes[16];
	void http_init(conn_rec *,client_request *);
	int filedir_check(char *);
};
