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

#include <include/neweraHPC.h>

using namespace std;
using namespace neweraHPC;

int main(int argc,char **argv)
{
   if(argc < 3)
   {
      cout<<"Usage: server 'hostname' 'port number'"<<endl;
      exit(0);
   }
   
   cout<<"Starting Thread Manager: ";
   thread_manager_t thread_manager;
   cout<<"\t\t\t OK"<<endl;
   
   cout<<"Starting Plugin Manager: ";
   plugin_manager_t plugin_manager(&thread_manager);
   cout<<"\t\t\t OK"<<endl;

   nhpc_status_t nrv = plugin_manager.install_plugin("samples/plugin.nxi");
   if(nrv == NHPC_SUCCESS)
      cout<<"Plugin Installed"<<endl;
   else
      cout<<"Plugin Installation Failed"<<endl;
      
   network_t network(&thread_manager);
   nrv = network.create_server(argv[1], argv[2], AF_INET, SOCK_STREAM, 0);
   if(nrv != NHPC_SUCCESS)
   {
      perror("error at creating server");
      exit(1);
   }
   
   return 0;
}
