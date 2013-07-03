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

#ifndef _NHPC_ACCEPT_H_
#define _NHPC_ACCEPT_H_

using namespace std;

void nhpc_accept_event(nhpc_event_t *ev);

static inline void nhpc_accept_free_connection(nhpc_connection_t *c) {
   nhpc_insert_queue(c->ls->connections_queue, c);
   if(c->pool)
      nhpc_destroy_pool(c->pool);
   c->pool = NULL;
   c->rev->active = 0;
   c->wev->active = 0;
   c->socket.fd   = 0;
}
static inline void nhpc_accept_close_connection(nhpc_connection_t *c) {
   if(!c->rev->active) {
      LOG_ERROR("failed to close connection");
      exit(0);
   }
   nhpc_shutdown_connection(c, SHUT_RDWR);
   nhpc_close_connection(c);
   nhpc_accept_free_connection(c);
}

#endif