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

#include <include/nhpc_general.h>

using namespace std;

nhpc_buffer_t *nhpc_buffer_init(nhpc_size_t size) {
   nhpc_buffer_t *buffer = (nhpc_buffer_t *)nhpc_alloc(sizeof(nhpc_buffer_t) + size);
   buffer->max = size;
   buffer->len = 0;
   
   pthread_mutex_init(&buffer->mutex, NULL);
   pthread_cond_init(&buffer->cond, NULL);
   
   return buffer;
}

u_char *nhpc_buffer_get_data(nhpc_buffer_t *buffer, nhpc_size_t *len) {
   u_char *data = (u_char *)buffer + sizeof(nhpc_buffer_t);
   int rv = 0;
   
   pthread_mutex_lock(&buffer->mutex);
   while(nhpc_buffer_is_empty(buffer)) {
      pthread_cond_wait(&buffer->cond, &buffer->mutex);
   }
   *len = buffer->len;
   pthread_mutex_unlock(&buffer->mutex);
   
   return data;
}

u_char *nhpc_buffer_get_dataptr(nhpc_buffer_t *buffer, nhpc_size_t *len) {
   u_char *data = (u_char *)buffer + sizeof(nhpc_buffer_t);
   
   pthread_mutex_lock(&buffer->mutex);
   while(!nhpc_buffer_is_empty(buffer)) {
      pthread_cond_wait(&buffer->cond, &buffer->mutex);
   }
   *len = buffer->max;
   pthread_mutex_unlock(&buffer->mutex);

   return data;
}

void nhpc_buffer_set_length(nhpc_buffer_t *buffer, nhpc_size_t l) {
   pthread_mutex_lock(&buffer->mutex);
   buffer->len = l;			 
   pthread_cond_broadcast(&buffer->cond);   
   pthread_mutex_unlock(&buffer->mutex); 
}
