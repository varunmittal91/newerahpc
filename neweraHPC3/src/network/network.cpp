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

#include <errno.h>
#include <iostream>

#include <include/network.h>

using namespace std;

namespace neweraHPC
{
   network_t::network_t()
   {
      external_thread_manager = false;
      thread_manager = new thread_manager_t;
      connection_stat = false;
   }
   network_t::network_t(thread_manager_t *in_thread_manager)
   {
      external_thread_manager = true;
      thread_manager = in_thread_manager;
      connection_stat = false;
   }
   network_t::~network_t()
   {
      if(!external_thread_manager)
	 delete thread_manager;
   }
};
