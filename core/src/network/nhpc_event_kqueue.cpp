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

#include <include/neweraHPC.h>

#ifdef HAVE_KQUEUE

nhpc_event_actions_t nhpc_event_actions = {
   nhpc_event_kqueue_add_event,
   nhpc_event_kqueue_del_event,
   nhpc_event_kqueue_process_changes,
   nhpc_event_kqueue_init,
   nhpc_event_kqueue_done,
};

int kq;
struct kevent *changelist;
struct kevent *eventlist;
int nchanges  = 0;
int nevents   = 0;
int maxevents = 0;

pthread_mutex_t mutex;

using namespace std;

nhpc_status_t nhpc_event_kqueue_set_event(nhpc_event_t *ev, nhpc_int_t filter, nhpc_uint_t flags) {
   struct kevent     *kev;  
   nhpc_connection_t *c;
   
   if(nchanges >= maxevents) {
      return NHPC_FAIL;
   }
   
   kev = &changelist[nchanges];
   c   = (nhpc_connection_t *)ev->data;
   
   kev->ident  = c->socket.fd;
   kev->filter = filter;
   kev->flags  = flags;
   kev->udata  = ev;
   
   ev->index = nchanges;
   nchanges++;

   return NHPC_SUCCESS;
}

nhpc_status_t nhpc_event_kqueue_add_event(nhpc_event_t *ev, nhpc_int_t event, nhpc_uint_t flags) {
   nhpc_status_t nrv;

   ev->index     = 0;
   ev->write     = 0;
   ev->accept    = 0;
   ev->active    = 1;
   ev->ready     = 0;
   ev->available = 0;
   ev->eof       = 0;
   
   pthread_mutex_lock(&mutex);
   nrv = nhpc_event_kqueue_set_event(ev, event, flags | EV_ADD | EV_ENABLE);
   pthread_mutex_unlock(&mutex);

   return nrv;
}
   
nhpc_status_t nhpc_event_kqueue_del_event(nhpc_event_t *ev, nhpc_int_t event, nhpc_uint_t flags) {
   pthread_mutex_lock(&mutex);

   if(flags & NHPC_DISABLE_EVENT) {
      flags |= EV_DISABLE;
      nhpc_event_kqueue_set_event(ev, event, flags);
   }
   else {
      nchanges--;
      if(ev->index < nchanges) {
	 changelist[ev->index] = changelist[nchanges];
	 nhpc_event_t *e = (nhpc_event_t *)changelist[ev->index].udata;
	 e->index = ev->index;
      }
   }
   
   pthread_mutex_unlock(&mutex);
   return NHPC_SUCCESS;   
}

void *nhpc_event_kqueue_exec(void *data) {
   nhpc_event_t *ev = (nhpc_event_t *)data;
   
   ev->handler(ev);
   nhpc_exit_thread();
}

nhpc_status_t nhpc_event_kqueue_process_changes(nhpc_listening_t *ls) {
   int    rv;
   struct timespec timedout = {0, 100 * 1000000};

   nhpc_connection_t *c;
   nhpc_event_t      *ev;
   nhpc_uint_t        events;
   pthread_t          tid;
   
   for(;;) {
      pthread_mutex_lock(&mutex);
      events = kevent(kq, changelist, nchanges, eventlist, nevents, &timedout);      
      pthread_mutex_unlock(&mutex);
      
      if(events == -1) {
	 LOG_ERROR("kevent() failed");
	 return NHPC_FAIL;
      } else if(events > 0) {
	 for(int i = 0; i < events; i++) {
	    
	    if(eventlist[i].data & EV_ERROR) {
	       cout << "Error in list" << endl;
	    }
	    
	    if(!(ev = (nhpc_event_t *)eventlist[i].udata)) {
	       LOG_ERROR("kevent() udata empty");
	       continue;
	    }
	    
	    if(ev->available) {
	       continue;
	    } else if(!ev->accept) {
	       ev->available = eventlist[i].data;
	       ev->handler(ev);
	    } else {
	       ev->available = eventlist[i].data;
	       ev->handler(ev);
	    }
	 }
      }
   }
}

nhpc_status_t nhpc_event_kqueue_init(nhpc_listening_t *ls) {
   if((kq = kqueue()) == -1)
      return errno;
   
   maxevents  = ls->nconnections + 1;
   nevents    = maxevents;
   nchanges   = 0;
   changelist = (struct kevent *)nhpc_alloc(sizeof(struct kevent) * maxevents);
   eventlist  = (struct kevent *)nhpc_alloc(sizeof(struct kevent) * maxevents);
   
   if(!changelist) 
      return NHPC_FAIL;
   
   nhpc_event_t *ev = &ls->events[0];
   ev->accept       = 1;
   ev->active       = 1;
   ev->handler      = nhpc_accept_event;
   ev->data         = ls;
   
   struct kevent *kev = changelist;
   kev->ident     = ls->socket.fd;
   kev->filter    = EVFILT_READ;
   kev->flags     = EV_ADD | EV_ENABLE;
   kev->udata     = ev;
   nchanges++;
   
   pthread_mutex_init(&mutex, NULL);
   
   return NHPC_SUCCESS;
}

void nhpc_event_kqueue_done(nhpc_listening_t *ls) {
   
}

#endif
