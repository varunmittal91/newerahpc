/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
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

#include <stdlib.h>
#include <iomanip>

#include <neweraHPC/network.h>
#include <neweraHPC/neweraHPC.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/grid_node.h>
#include <include/grid_server.h>
#include <include/grid_communication.h>
#include <include/grid_controller_register.h>
#include <include/grid_plugin.h>
#include <include/grid_scheduler.h>
#include <include/grid_tmpfs.h>

using namespace std;

namespace neweraHPC
{
   network_t        *_network;
   thread_manager_t *_thread_manager;
   
   const char *_host_addr;
   const char *_host_port;
   const char *_host_grid_uid;
   const char *_tmp_dir;
   int _host_core_count;
   int _host_cpu_time;   
   
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
   
   nhpc_status_t grid_server_init(int argc, char **argv)
   {
      neweraHPC_init(argc, argv);
      
      const char *host       = NULL;
      const char *controller = NULL;
      const char *cpu_time   = NULL;
      bool  daemon     = false;      
      
      const char *data;
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
	 _host_cpu_time = nhpc_strtoi(cpu_time);
	 if(_host_cpu_time > 100)
	    _host_cpu_time = 100;
      }
      else 
	 _host_cpu_time = 0;   
      _host_core_count = sysconf(_SC_NPROCESSORS_CONF);
      
      if(host)
      {
	 string_t *string = nhpc_substr(host, ':');
	 nhpc_strcpy((char **)&_host_addr, string->strings[0]);
	 
	 if(string->count == 1)
	 {
	    nhpc_strcpy((char **)&_host_port, "8080");
	 }
	 else 
	    nhpc_strcpy((char **)&_host_port, string->strings[1]);	 
	 
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
	    nhpc_strcpy((char **)&controller_port, "8080");
	 else 
	    nhpc_strcpy((char **)&controller_port, string->strings[1]);
	 
	 const char *grid_uid;
	 nhpc_status_t nrv = grid_controller_register_to_server(&grid_uid, controller_addr, controller_port);
	 
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
      
      nhpc_system_init();
      
      _thread_manager = new thread_manager_t;
      _network        = new network_t(&_thread_manager);
      (*_network).network_init();
      
      int rv;
      fnc_ptr_t grid_handler = (fnc_ptr_t)grid_request_handler;
      rv = (*_network).network_addons->insert((void *)grid_handler, "GRID");

      nhpc_status_t nrv = (*_network).create_server(_host_addr, _host_port, AF_INET, SOCK_STREAM, 0);  
      if(nrv != NHPC_SUCCESS)
	 return nrv;  
      
      grid_communication_handlers_init();
      grid_node_db_init();
      grid_plugin_system_init();
      grid_scheduler_system_init();
      grid_tmpfs_init();
      
      (*_network).join_accept_thread();
   }
}
