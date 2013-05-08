/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
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

#ifndef _GRID_INSTRUCTION_DATA_TYPE_H_
#define _GRID_INSTRUCTION_DATA_TYPE_H_

namespace neweraHPC
{
   typedef unsigned char instruction_status_t;
   struct grid_instruction_t
   {
      const char *plugin_name;
      rbtree     *arguments;
      
      const char *grid_uid;
      const char *peer_addr;
      const char *peer_port;
      
      const char *referer_grid_uid;
      const char *referer_addr;
      const char *referer_port;
      
      grid_shared_data_t *input_data;
      grid_shared_data_t *result_data;
      
      instruction_status_t  instruction_status;
   };   
};

#endif