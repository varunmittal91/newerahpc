/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v2
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 2 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <iomanip>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/neweraHPC.h>
#include <include/grid.h>
#include <include/network.h>
#include <include/thread.h>
#include <include/file.h>
#include <include/system.h>

using namespace std;

namespace neweraHPC
{
   nhpc_grid_server_t *grid_server;
   
   nhpc_grid_server_t::nhpc_grid_server_t(const char *in_host, const char *in_cpu_time) 
   : network_t(&thread_manager), plugin_manager_t(&thread_manager), grid_scheduler_t(&thread_manager)
   {
      clients = new rbtree(RBTREE_STR);
      thread_manager = new thread_manager_t;
      
      string_t *string = nhpc_substr(in_host, ':');
      nhpc_strcpy(&host_addr, string->strings[0]);
      
      if(string->count == 1)
      {
	 nhpc_strcpy(&host_port, "8080");
      }
      else 
	 nhpc_strcpy(&host_port, string->strings[1]);  
      
      if(in_cpu_time)
      {
	 host_cpu_time = nhpc_strtoi(in_cpu_time);
	 if(host_cpu_time > 100)
	    host_cpu_time = 100;
      }
      else 
	 host_cpu_time = 0;
      
      host_cores = sysconf(_SC_NPROCESSORS_CONF);
      
      nhpc_string_delete(string);
   }
   
   nhpc_grid_server_t::nhpc_grid_server_t() 
   : network_t(&thread_manager), plugin_manager_t(&thread_manager), grid_scheduler_t(&thread_manager)
   {
      clients = new rbtree(RBTREE_STR);
      thread_manager = new thread_manager_t;
    
      host_cpu_time = 0;
      host_cores = sysconf(_SC_NPROCESSORS_CONF);
      
      host_port = NULL;
      host_addr = NULL;
   }      
   
   void print_help()
   {
      cout<<"Usage: server \t[-l host_ip:port] [-r remote_ip:port] \n\t\t[-c cpu_time] [-d daemon]"<<endl;
      cout<<"Options:"<<endl;
      cout<<setw(20)<<"-l host_ip:port"<<setw(50)<<":Ip address and port of local server"<<endl;
      cout<<setw(22)<<"-r remote_ip:port"<<setw(46)<<":Ip address and port of controller"<<endl;
      cout<<setw(16)<<"-c cpu_time"<<setw(38)<<":Mac cpu time to use"<<endl;
      cout<<setw(14)<<"-d daemon"<<setw(32)<<":Daemon mode"<<endl;
      cout<<setw(15)<<"-v verbose"<<setw(50)<<":Verbose mode: info, debug, all"<<endl;
      cout<<setw(12)<<"-h help"<<setw(37)<<":This help menu"<<endl;
      
      exit(0);
   }   
   
   nhpc_status_t nhpc_grid_server_t::grid_server_init()
   {
      grid_server = this;
      
      nhpc_status_t nrv;
      
      char *host = NULL;
      char *controller = NULL;
      char *cpu_time = NULL;
      bool daemon = false;      
      
      char       *data;
      const char *key;

      int argument_count = nhpc_get_cmdline_argument_count();
      
      for(int i = 1; i <= argument_count; i++)
      {
	 data = nhpc_get_cmdline_argument_key(i, key);
	 
	 switch(*key)
	 {
	    case 'c':
	       cpu_time = data;
	       break;
	    case 'l':
	       host = data;
	       break;
	    case 'r':
	       controller = data;
	       break;
	    case 'h':
	       print_help();
	       break;
	    case 'd':
	       daemon = true;
	       break;
	    default:
	       LOG_ERROR("Invalid command line option: " << key);
	       print_help();
	 }	 
      }

      if(cpu_time)
      {
	 host_cpu_time = nhpc_strtoi(cpu_time);
	 if(host_cpu_time > 100)
	    host_cpu_time = 100;
      }
      else 
	 host_cpu_time = 0;      
      
      if(host)
      {
	 string_t *string = nhpc_substr(host, ':');
	 nhpc_strcpy(&host_addr, string->strings[0]);
	 
	 if(string->count == 1)
	 {
	    nhpc_strcpy(&host_port, "8080");
	 }
	 else 
	    nhpc_strcpy(&host_port, string->strings[1]);	 
	 
	 nhpc_string_delete(string);
      }
      else 
      {
	 LOG_ERROR("Host not specified");
	 print_help();
      }
	 
      if(controller)
      {
	 string_t *string = nhpc_substr(controller, ':');

	 const char *controller_addr = (const char *)(string->strings[0]);
	 const char *controller_port;
	 
	 if(string->count == 1)
	    controller_port = "8080";
	 else 
	    controller_port = string->strings[1];
	 
	 nhpc_strcpy(&grid_controller_addr, controller_addr);
	 nhpc_strcpy(&grid_controller_port, controller_port);
	 
	 LOG_INFO("Registering to controller: "<<grid_controller_addr<<":"<<grid_controller_port);
	 
	 nhpc_status_t nrv = nhpc_register_to_controller(grid_controller_addr, grid_controller_port, host_addr, host_port, 
							 host_cores, host_cpu_time);	 
	 if(nrv == NHPC_FAIL)
	    LOG_ERROR("Registration to the controller failed\n Running without controller");

	 nhpc_string_delete(string);
      }

      if(daemon)
      {
	 int rv;
	 
	 int pid = fork();
	 if(pid != 0)
	    exit(0);
	 setsid();
	 
	 int i;
	 
	 for (i = getdtablesize(); i >= 0; --i)
	    close(i);
	 
	 i = open("/dev/null", O_RDWR);
	 rv = dup(i);
	 rv = dup(i);
      }  
      
      jobs = new rbtree(RBTREE_STR);
      
      nhpc_system_init();
      plugin_manager_init();
      grid_scheduler_init();
            
      mkdir("/tmp/neweraHPC", 0777);
      
      fnc_ptr_t grid_request_handler = (fnc_ptr_t)nhpc_grid_server_t::grid_request_init;
      int rv = network_addons->insert((void *)grid_request_handler, "GRID");

      add_peer(host_addr, host_port, host_cores, host_cpu_time);

      nrv = create_server(host_addr, host_port, AF_INET, SOCK_STREAM, 0);  
      if(nrv != NHPC_SUCCESS)
	 return nrv;
      
      grid_communication_handlers = new rbtree(RBTREE_STR);
      (*grid_communication_handlers).insert((void *)(nhpc_grid_server_t::grid_client_registration_handler), 
					    nhpc_grid_get_communication_code_status_mssg(GRID_CLIENT_REGISTRATION));
      (*grid_communication_handlers).insert((void *)(nhpc_grid_server_t::grid_node_registration_handler), 
					    nhpc_grid_get_communication_code_status_mssg(GRID_NODE_REGISTRATION));
      (*grid_communication_handlers).insert((void *)(nhpc_grid_server_t::grid_plugin_request_handler), 
					    nhpc_grid_get_communication_code_status_mssg(GRID_PLUGIN_REQUEST));
      (*grid_communication_handlers).insert((void *)(nhpc_grid_server_t::grid_file_exchange_request_handler), 
					    nhpc_grid_get_communication_code_status_mssg(GRID_FILE_EXCHANGE));
      (*grid_communication_handlers).insert((void *)(nhpc_grid_server_t::grid_instruction_request_handler), 
					    nhpc_grid_get_communication_code_status_mssg(GRID_INSTRUCTION));
      
      int plugin_request_thread_id;
      thread_manager->init_thread(&plugin_request_thread_id, NULL);
      thread_manager->create_thread(&plugin_request_thread_id, NULL, (void* (*)(void*))nhpc_grid_server_t::grid_plugin_request_thread, 
				    this, NHPC_THREAD_DEFAULT);      
   }
   
   void nhpc_grid_server_t::grid_server_join()
   {
      join_accept_thread();
   }
   
   nhpc_grid_server_t::~nhpc_grid_server_t()
   {
      LOG_INFO("Shuting Down Grid Server");
      
      nhpc_system_destroy();
      nhpc_string_delete(host_addr);
      nhpc_string_delete(host_port);
      
      LOG_INFO("Deleting grid client details");

      char       *tmp_host_addr;
      const char *key_str;

      int client_count = (*clients).length();
      for(int i = 1; i <= client_count; i++)
      {
	 tmp_host_addr = (char *)(*clients)[i];
	 delete[] tmp_host_addr;
      }
      
      delete clients;
   }
   
   void nhpc_grid_server_t::grid_request_init(nhpc_socket_t *sock)
   {
      char       *command = network_headers_get_param(sock->headers, "command");
      const char *uid     = network_headers_get_param(sock->headers, "Grid-Uid");
      string_t   *string  = nhpc_substr(command, ' ');
      
      if(string->count < 3)
      {
	 nhpc_string_delete(string);
	 return;
      }
      
      nhpc_status_t  nrv     = NHPC_FAIL;
      const char    *fnc_str = string->strings[1];
      
      fnc_ptr_nhpc_two_t fnc_ptr =(fnc_ptr_nhpc_two_t)grid_communication_handlers->search(fnc_str);
      
      if(fnc_ptr)
      {
	 nrv = fnc_ptr(grid_server, sock);
      }
      
      LOG_INFO("Executed function with status: " << nrv);
      
      nhpc_string_delete(string);
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_client_registration(nhpc_socket_t *sock)
   {
      nhpc_status_t nrv;
      nrv = NHPC_FAIL;
      
      char *uid;
      nrv = grid_client_gen_uid(sock->host, (const char **)&uid);
      
      nhpc_size_t size = strlen(uid);
      nrv = socket_sendmsg(sock, uid, &size);
      
      char *dir = nhpc_strconcat(grid_directory, uid);
      mkdir(dir, 0777);
      nhpc_string_delete(dir);
      nhpc_string_delete(uid);
      
      return nrv;      
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_node_registration(nhpc_socket_t *sock)
   {
      char *node_addr     = network_headers_get_param(sock->headers, "Peer-Host");
      char *node_port     = network_headers_get_param(sock->headers, "Peer-Port");
      char *node_cores    = network_headers_get_param(sock->headers, "Node-Cores");
      char *node_cpu_time = network_headers_get_param(sock->headers, "Node-Cpu-Time");
      
      if(!node_addr || !node_port || !node_cores || !node_cpu_time)
	 return NHPC_FAIL;
      
      add_peer(sock->host, node_port, nhpc_strtoi(node_cores), nhpc_strtoi(node_cpu_time));
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_client_gen_uid(const char *client_addr, const char **uid)
   {
      char *tmp_client_addr;
      char *random_string = NULL;
      
      do 
      {
	 if(random_string != NULL)
	    nhpc_string_delete(random_string);
	 
	 random_string = nhpc_random_string(8);
	 
	 tmp_client_addr = (char *)clients->search(random_string);
      }while(tmp_client_addr != NULL);
      
      *uid = random_string;
      nhpc_strcpy(&tmp_client_addr, client_addr);
      clients->insert((void *)tmp_client_addr, random_string);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_grid_server_t::grid_client_verify_uid(const char *uid)
   {      
      if(clients->search(uid) == NULL)
	 return NHPC_FAIL;
      
      return NHPC_SUCCESS;
   }
      
   void nhpc_grid_server_t::grid_plugin_request_thread(nhpc_grid_server_t *grid_server)
   {
      plugin_manager_t plugin_manager = (plugin_manager_t)(*grid_server);
      
      plugin_request_t *plugin_request;
      const char       *grid_uid;
      nhpc_headers_t   *headers;
      nhpc_status_t     nrv;
      nhpc_socket_t    *sock;
      char             *peer_host;
      char             *peer_port;
      
      while(1)
      {
	 plugin_request = plugin_manager.return_plugin_request();
	 if(plugin_request)
	 {
	    peer_host = plugin_request->peer_host;
	    peer_port = plugin_request->peer_port;
	    
	    grid_communication_t *grid_communication;
	    nhpc_grid_communication_init(&grid_communication, GRID_PLUGIN_REQUEST);
	    nhpc_grid_set_communication_opt(grid_communication, GRID_COMMUNICATION_REGISTER);
	    nhpc_grid_communication_add_dest(grid_communication, peer_host, peer_port);
	    nhpc_grid_communication_add_peer(grid_communication, grid_server);
	    nhpc_grid_communication_send(grid_communication);
	    
	    grid_communication->headers->insert("Plugin", plugin_request->plugin);
	    
	    nrv = nhpc_grid_communication_push(grid_communication);
	    if(nrv == NHPC_SUCCESS)
	       nhpc_grid_set_plugin_request_complete(plugin_request);
	    
	    nhpc_grid_communication_destruct(grid_communication);
	 }
	 sleep(1); 
      }
   }
};
