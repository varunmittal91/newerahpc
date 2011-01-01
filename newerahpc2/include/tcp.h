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

#include <map>

#define MAXPENDING 10
#define MAXCONN 10
#define BUFFSIZE 1024
#define CLIENT_REQ_MAX 1
#define SERVER_PORT 8091
#define NO_FILE 0
#define GET_FILE 1
#define NEWERA 1

namespace newera_network{
	struct plugin_manager_t{
		char **plugins;
		int count;
	};
	struct conn_details{
		struct link{
			pthread_t thread;
			int cid;
			link *next_link;
			char *data;
			void *data_univ;
		};
		link *next_link;
		int count;
		int curr_cid;
	};
	typedef std::map<std::string,conn_details *>clients_t;
	class conn_rec{
	public:
		int sockfd;                                                  
		char *host;
		int port;
		int servr_port;
		int cid;
		struct sockaddr_in addr;
		struct hostent *host_info;
		bool conn_status;
	};
	class conn_database{
		pthread_mutex_t mutex;
		clients_t clients;
		plugin_manager_t *plugin_manager;
	public:
		conn_database();
		~conn_database();
		int add(char *,pthread_t);
		int add(char *,pthread_t,void *);
		void remove(char *,int);
		void display(char *);
		conn_details::link *return_details(char *,int);
	};
	class client_request{
	public:
		char **lines;
		char *left_over;
		int *length;
		int count;
		int limit;
		int get_file;
		int file_size;
		int file_size_act;
		std::ofstream fp;
		std::string file_location;
		void *grid_data_v;
		client_request();
		~client_request();
		void increase_limit();
		void read(conn_rec *);
		void read_file(conn_rec *,char *,int);
	};
	class network_write{
		char **buffer;
		int *lengths;
		int sockfd;
		int lines;
		int patch;
		FILE *fp;
	public:
		network_write(int);		
		~network_write();
		void add(const char *);
		void add(std::string *);
		int push();
		int push_http();
		void push_raw(char *,int);
		int push_file();
		void reset();
		void file_assign(char *);
	};
	extern conn_rec *local_rec;
	extern conn_database *database;
	extern int server_port;
	void *open_socket(void *);
	void connect(conn_rec *);
	void connection_close(conn_rec *);
	void start();
};
