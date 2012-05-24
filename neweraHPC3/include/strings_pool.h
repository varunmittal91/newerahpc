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

#ifndef _STRINGS_POOL_H_
#define _STRINGS_POOL_H_

#include "rbtree.h"

namespace neweraHPC
{   
   class strings_pool_t
   {
   private:
      rbtree_t *strings_free;
      rbtree_t *strings_allocated;
      
      struct pool_string_t
      {
	 void *address;
	 nhpc_size_t len;
      };
   public:
      strings_pool_t();
      ~strings_pool_t();
      
      char *search_string(nhpc_size_t str_len);
      void free_string(char *address);
   };
};

#endif