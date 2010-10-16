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

typedef void *(*func_ptr)(void *);
struct func_details{
	func_ptr ptr;
	func_ptr ptr_client;
	func_ptr ptr_processor;
	char *path;
};
struct peer_details{
	int port;
	std::string host;
	int processors;
	int status;
};
typedef std::map<std::string,func_details *> functions_map;
typedef std::map<std::string,peer_details *> peers;

#define conf_file "/etc/grid_node.cfg"
#define GET_GRID 2
#define GRID_DONE 1
#define GRID_NDONE 2

using namespace std;

namespace newera_network{
	class instruction_set;
	class config{
		peers peers_t;
		pthread_mutex_t *thread;
	public:
		config();
		~config();
		void add_peer(const char *,int,int);
		peer_details *return_peer();
		void lock_config();
		void unlock_config();
		void free_peer(string);
	};
	class grid_task{
		func_ptr *function;
		int count;
		int used;
		pthread_mutex_t *mutex;
		bool status;
		void **data;
		void lock();
		void unlock();
	public:
		char *func_name;
		instruction_set **instructions;
		int c_id;
		grid_task();
		~grid_task();
		void operator=(instruction_set *);
		instruction_set *return_instruction();
		void remove(instruction_set *);
		int size(){
			return count;
		}
		bool check();
		void submit();
		void collect();
	};
	class instruction_set{
	public:
		char *host;
		int port;
		int id;
		int status;
		char *plugin;
		size_t length;
		void *data;
		pthread_t thread;
		char *func_name;
		grid_task *task;
		instruction_set();
	};
	class grid_data{
		void *data_final;
		char **data;
		size_t *lengths;
		size_t length;
		size_t count;
		void expand();
	public:
		grid_data();
		~grid_data();
		void add(char *,size_t);
		void display();
		void *data_return();
	};
	class task_manager{
		int count;
		int c_id;
		grid_task **tasks;
		pthread_mutex_t mutex;
	public:
		task_manager();
		~task_manager();
		int add_task(grid_task *);
		grid_task *return_task(int cid);
		void remove_task();
		void lock_task();
		void unlock_task();
	};
	class plugin_manager{
	private:
		struct plugin_request{
			char *host;
			int port;
			char *plg_name;
			char *plg_code;
			plugin_request *next;
		};
		plugin_request *requests;
		int request_count;
		pthread_mutex_t *mutex;
		void lock_plugin();
		void unlock_plugin();
		bool check_request(char *,char *);
	public:
		functions_map functions;
		plugin_manager();
		~plugin_manager();
		void add_request(conn_rec *,char *,char *);
		void wait_plugin();
		void load(char *);
		bool check_dll(char *);
		void display_plugin_requests();
		void add_nxi(const char *);
		void read_nxi(const char *);
		void create_nxi(const char *);
	};
	class newera_hpc:public config,public task_manager,public plugin_manager{
		grid_task *tasks;
		pthread_mutex_t mutex;
	public:
		newera_hpc();
		~newera_hpc();
		void lock();
		void unlock();
		void add();
		void *execute(char *);
		void execute_client(instruction_set *);
		char *return_path(char *);
		void grid_lng_init(network_write *,char *);
		static void *send_job(void *);
	};
	extern newera_hpc *hpc_data;
	void grid_init(conn_rec *,client_request *);
	void grid_execute(char *);
	void load_node_list();
};


