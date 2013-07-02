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

#include <arpa/inet.h>
#include <pthread.h>

#include <include/neweraHPC.h>

using namespace std;

void nhpc_init_connection(nhpc_connection_t *c) {
   
}

void nhpc_destroy_connection(nhpc_connection_t *c) {
   
}

void nhpc_init_listening(nhpc_listening_t *ls) {
   ls->nconnections = CONNECTION_BACKLOG;
   ls->connections  = (nhpc_connection_t *)nhpc_calloc(sizeof(nhpc_connection_t) * ls->nconnections);
   ls->events       = (nhpc_event_t *)nhpc_calloc(sizeof(nhpc_event_t) * (ls->nconnections * 2 + 7));

   ls->connections_queue = nhpc_init_queue(CONNECTION_BACKLOG);
   
   nhpc_event_t *rev, *wev;
   
   for(int i = 0; i < CONNECTION_BACKLOG; i++) {
      rev = &ls->events[2 * i + 1];
      wev = &ls->events[2 * i + 2];
      rev->data = &ls->connections[i];
      wev->data = &ls->connections[i];
      rev->instance = 1;
      wev->instance = 1;
      
      ls->connections[i].rev = rev;
      ls->connections[i].wev = wev;      
      ls->connections[i].ls  = ls;

      nhpc_insert_queue(ls->connections_queue, &ls->connections[i]);
   }
}

void nhpc_destroy_listening(nhpc_listening_t *ls) {
   
}

void nhpc_get_addr_info(nhpc_connection_t *c) {
   nhpc_strcpy(c->peer.addr, inet_ntoa(c->socket.sa_in.sin_addr));
   nhpc_itoa(c->peer.port, ntohs(c->socket.sa_in.sin_port));
}

