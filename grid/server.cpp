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
#include <iostream>
#include <iomanip>
#include <fstream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <neweraHPC/neweraHPC.h>

#include <include/grid_server.h>
#include <include/grid_data.h>

using namespace std;
using namespace neweraHPC;

int main(int argc, char **argv)
{      
   nhpc_status_t nrv = grid_server_init(argc, argv);
   if(nrv == NHPC_SUCCESS)
      grid_server_join();
}
