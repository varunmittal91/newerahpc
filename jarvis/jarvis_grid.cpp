/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com> & Uday Vir Singh <udayvirsingh@gmail.com>
 *	jarvis program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of jarvis.
 *
 *	jarvis is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	jarvis is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with jarvis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <neweraHPC/neweraHPC.h>
#include <neweraHPC/grid_server.h>
#include <neweraHPC/grid_plugin.h>

#include <include/jarvis.h>
#include <include/web_interface.h>

using namespace std;
using namespace jarvis;
using namespace neweraHPC;

int main(int argc, char **argv)
{
   nhpc_status_t nrv;
   
   nrv = grid_server_init(argc, argv);
   if(nrv == NHPC_SUCCESS)
   {
      const char       *plugin_path;
      plugin_details_t *plugin;

#ifdef __APPLE__
      plugin_path = "src/.libs/libjarvis.0.dylib";
#else
      plugin_path = "src/.libs/libjarvis.0.so";
#endif
      
      nrv = grid_plugin_install_dll(plugin_path, &plugin);
      if(nrv == NHPC_SUCCESS)
	 cout << "Plugin accepted in the core" << endl;
      else 
      {
	 cout << "Plugin declined" << endl;
      }
      
      jarvis_init(argc, argv);
      web_interface_init();
      grid_server_join();
   }

   return 0;
}