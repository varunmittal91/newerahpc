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

#include <stdlib.h>
#include <iostream>
#include <iomanip>

#include <include/neweraHPC.h>

using namespace std;
using namespace neweraHPC;

void print_help()
{
   cout<<"Usage: server \t[-l host_ip:port] [-r remote_ip:port] \n\t\t[-c cpu_time] [-d daemon]"<<endl;
   cout<<"Options:"<<endl;
   cout<<setw(20)<<"-l host_ip:port"<<setw(50)<<":Ip address and port of local server"<<endl;
   cout<<setw(22)<<"-r remote_ip:port"<<setw(46)<<":Ip address and port of controller"<<endl;
   cout<<setw(16)<<"-c cpu_time"<<setw(38)<<":Mac cpu time to use"<<endl;
   cout<<setw(14)<<"-d daemon"<<setw(32)<<":Daemon mode"<<endl;
   cout<<setw(12)<<"-h help"<<setw(37)<<":This help menu"<<endl;
   
   exit(0);
}

int main(int argc, char **argv)
{
   char *host = NULL;
   char *controller = NULL;
   char *cpu_time = NULL;
   bool daemon = false;
   
   if(argc == 1)
   {
      print_help();
      exit(0);
   }
   
   char **tmp_argv = argv + 1;
   while(*tmp_argv != NULL)
   {
      char *arg = *tmp_argv;
      arg++;
      switch(*arg)
      {
         case 'c':
	    cpu_time = *(tmp_argv + 1);
            tmp_argv++;
            break;
         case 'l':
	    host = *(tmp_argv + 1);
            tmp_argv++;
            break;
	 case 'r':
	    controller = *(tmp_argv + 1);
	    tmp_argv++;
	    break;
	 case 'h':
	    print_help();
	    break;
	 case 'd':
	    daemon = true;
	    break;
	 default:
	    print_help();
      }
      tmp_argv++;
   }   
   
   if(daemon)
   {
      int pid = fork();
      if(pid != 0)
	 exit(0);
      setsid();
      
      int i;
      
      for (i=getdtablesize();i>=0;--i) close(i);
      
      i=open("/dev/null",O_RDWR);
      dup(i);
      dup(i);
   }
   
   nhpc_status_t nrv;
   
   nhpc_grid_server_t grid_server(host, cpu_time);
   if(controller)
      nrv = grid_server.grid_server_init(controller);
   else 
      nrv = grid_server.grid_server_init();
   
   if(nrv != NHPC_SUCCESS)
      cout<<"Grid initialization failed\n"<<endl;
   
   return 0;
}
