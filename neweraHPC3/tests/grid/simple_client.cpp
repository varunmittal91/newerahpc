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

int main(int argc, char **argv)
{
   if(argc < 3)
   {
      cout<<"Usage: ./simple_client server_host server_port\n";
      return 1;
   }
   
   const char *grid_uid;
   
   nhpc_status_t nrv = nhpc_register_to_server(&grid_uid, argv[1], argv[2]);
   
   if(nrv != NHPC_SUCCESS)
   {
      cout<<"Registration failed with server\n";
      return 1;
   }
   else 
   {
      cout<<"Grid registration uid: "<<grid_uid<<endl;
      cout<<"Registration done\n";
   }
   
   nrv = nhpc_send_plugin(grid_uid, argv[1], argv[2], "../../samples/plugin.nxi");
   if(nrv != NHPC_SUCCESS)
   {
      cout<<"Plugin Upload Failed";
      return 1;
   }
   
   cout<<"Plugin Uploaded"<<endl;
   
   delete[] grid_uid;
   
   return 0;
}
