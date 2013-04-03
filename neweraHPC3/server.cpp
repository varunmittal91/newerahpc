/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
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
#include <fstream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/rbtree.h>
#include <include/grid.h>
#include <include/neweraHPC.h>
#include <include/heap.h>

using namespace std;
using namespace neweraHPC;

int main(int argc, char **argv)
{      
   neweraHPC_init(argc, argv);
   nhpc_status_t nrv;   
   
   const char *host_addr, *host_port;
   const char *host = nhpc_get_cmdline_argument("l");
   if(host)
   {
      string_t *host_parts = nhpc_substr(host, ':');
      nhpc_strcpy((char **)&host_addr, host_parts->strings[0]);
      if(host_parts->count == 1)
      {
	 host_port = "8080";
      }
      else 
	 nhpc_strcpy((char **)&host_port, host_parts->strings[1]);	 
      
      nhpc_string_delete(host_parts);
   }
   else 
   {
      host_addr = "localhost";
      host_port = "8080";
   }
   
   network_t *network = new network_t;
   (*network).network_init();
   http_init(network);
   nrv = (*network).create_server(host_addr, host_port, AF_INET, SOCK_STREAM, 0);
   if(nrv == NHPC_SUCCESS)
      (*network).join_accept_thread();

   return 0;
}
