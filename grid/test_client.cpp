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

#include <iostream>

#include <neweraHPC/constants.h>
#include <neweraHPC/neweraHPC.h>

#include <include/grid_client_registration.h>
#include <include/grid_instruction.h>

using namespace std;
using namespace neweraHPC;

int main(int argc, char **argv)
{
   neweraHPC_init(argc, argv);
   
   const char *grid_uid;
   
   nhpc_status_t nrv = grid_client_register_to_server(&grid_uid, "localhost", "8080");
   
   if(nrv != NHPC_SUCCESS)
   {
      cout<<"Registration failed with server\n";
      return 1;
   }
   else 
   {
      int a = 12;
      int b = 13;
      
      grid_instruction_t *instruction;
      grid_instruction_init(&instruction);
      grid_instruction_set_plugin_name(instruction, "GRID_PLUGIN_RANGE");
      grid_instruction_set_peer(instruction, "localhost", "8080");
      grid_instruction_add_argument(instruction, ARG_FILE, "blender");
      grid_instruction_add_argument(instruction, ARG_RANGE, &a, &b);
      
      cout<<"Grid registration uid: "<<grid_uid<<endl;
      cout<<"Registration done\n";
   }
}