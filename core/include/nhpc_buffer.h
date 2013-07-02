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

struct nhpc_buffer_t {
   nhpc_size_t      max;
   nhpc_size_t      len;
   pthread_mutex_t  mutex;
   pthread_cond_t   cond;
};

nhpc_buffer_t *nhpc_buffer_init(nhpc_size_t size);
void           nhpc_buffer_destroy(nhpc_buffer_t *buffer);

u_char *nhpc_buffer_get_dataptr(nhpc_buffer_t *buffer, nhpc_size_t *len);
u_char *nhpc_buffer_get_data(nhpc_buffer_t *buffer, nhpc_size_t *len);

void nhpc_buffer_set_length(nhpc_buffer_t *buffer, nhpc_size_t l);

#define nhpc_buffer_set_empty(b)  nhpc_buffer_set_length(b, 0)

#define nhpc_buffer_is_empty(b) (b->len == 0)
#define nhpc_buffer_is_full(b)  (b->len == b->max)
