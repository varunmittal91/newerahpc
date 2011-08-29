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

#include <include/neweraHPC.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

using namespace neweraHPC;
using namespace std;

int main(int argc, char *argv[]){
   int nrv;
   
   if(argc<4)
   {
      printf("usage ./test host port mode\n\t1 for client\n\t2 for server\n");
      exit(0);
   }
   
   network_t network;
   nhpc_socket_t *sock;
   
   if(strcmp(argv[3],"1") == 0)
   {
      nrv = network.connect(&sock, argv[1], argv[2], AF_INET, SOCK_STREAM, 0);
      if(nrv != NHPC_SUCCESS)
      {
	 perror("connect");
	 exit(0);
      }
      
      const char *mssg = "GET / HTTP/1.1 \r\n\r\n";
      size_t size = strlen(mssg);
      nrv = socket_send(sock, (char *)mssg, &size);
      if(nrv == -1)perror("write");
      
      char buffer[1000];
      size = 1000;
      int rv;
      
      int timeup_count = 0;
      while(rv != NHPC_EOF && timeup_count != 5)
      {
	 rv = socket_recv(sock, buffer, &size);
	 cout<<buffer;
	 timeup_count++;
      }
      cout<<endl;
      
      if(rv == -1)
	 perror("read:");
      
      socket_delete(sock);
      
   }
   if(strcmp(argv[3],"2") == 0)
   {
      //test_socket_factory();
      //while(1)sleep(1);
      nrv = network.create_server(argv[1], argv[2], AF_INET, SOCK_STREAM, 0);
      if(nrv != NHPC_SUCCESS)perror("error at creating server");
   }
   
   return 0;
}