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

nhpc_buffer_t *nhpc_buffer_init(nhpc_pool_t *p) {
   nhpc_buffer_t *buffer = (nhpc_buffer_t *)nhpc_pcalloc(p, sizeof(nhpc_buffer_t));

   buffer->pool  = p;
   buffer->start = NULL;
   buffer->end   = NULL;
   buffer->chain = NULL;
   
   return buffer;
}

void nhpc_buffer_destroy(nhpc_buffer_t *buffer) {
   
}

void nhpc_buffer_add_data(nhpc_buffer_t *buffer, u_char *address, nhpc_size_t data_len, 
			  nhpc_uint_t buffer_data_type, nhpc_uint_t deallocate) {
   buffer->address    = address;
   buffer->deallocate = deallocate;
   buffer->data_type  = buffer_data_type;
   
   if(buffer->data_type & NHPC_BUFFER_DATA_MEM_BLOCK) {

      nhpc_chain_t **chain = &(buffer->chain);
      while((*chain)) {
	 chain = &((*chain)->next);
      }
      (*chain) = (nhpc_chain_t *)nhpc_palloc(buffer->pool, sizeof(nhpc_chain_t));
      
      u_char *tmp_address;
      if(deallocate) {
	 tmp_address = (u_char *)nhpc_palloc(buffer->pool, data_len);
	 memcpy(tmp_address, address, data_len);
	 address = tmp_address;
	 
	 buffer->deallocate = 1;
      }
      
      (*chain)->next  = NULL;
      (*chain)->start = address;
      (*chain)->end   = address + data_len;
      
      if(!buffer->start) {
	 buffer->start = (*chain)->start;
	 buffer->end   = (*chain)->end;
      }
   }
   
   return;
}