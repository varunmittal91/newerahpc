/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

#include <include/neweraHPC.h>

using namespace std;

void *producer(void *data) {
   
}

int main(int argc, char **argv)
{
   nhpc_status_t nrv;
   
   neweraHPC_init(argc, argv);
   nhpc_init_network();
   nhpc_init_http();
   if((nrv = nhpc_create_server("0.0.0.0", "8080")) != NHPC_SUCCESS)
      perror("nhpc_create_server");
   
   while(1)
      sleep(1);

   pthread_t tid;   
}
