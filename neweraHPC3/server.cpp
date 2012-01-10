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
   int pid = fork();
   if(pid != 0)
      exit(0);
   
   if(argc < 4)
   {
      cout<<"Usage: server 'hostname' 'port number' 'core count'"<<endl;
      exit(0);
   }
   
   nhpc_grid_server_t grid_server(argv[1], argv[2], argv[3]);
   if(argc == 6)
      grid_server.grid_server_init(argv[4], argv[5]);
   else 
      grid_server.grid_server_init();
   
   return 0;
}
