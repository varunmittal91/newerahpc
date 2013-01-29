/*
 *
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

#include <iostream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/grid.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC 
{
   nhpc_status_t nhpc_grid_communication_send(grid_communication_t *grid_communication)
   {
      const char *mssg = nhpc_grid_get_communication_status_mssg(grid_communication);
      cout << mssg << endl;
   }
}