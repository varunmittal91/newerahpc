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

#ifndef _GRID_DATA_H_
#define _GRID_DATA_H_

namespace neweraHPC
{
#define ARG_RANGE     1
#define ARG_VALUE     2
#define ARG_LITERAL   4
#define ARG_COMMAND   8
#define ARG_FILE     16
#define ARG_RANGE_BIT    0
#define ARG_VALUE_BIT    1
#define ARG_LITERAL_BIT  2
#define ARG_COMMAND_BIT  3
#define ARG_FILE_BIT     4
   typedef unsigned char arg_t;
   struct arg_value_t
   {
      arg_t       arg;
      const char *value;
   };
   
   typedef unsigned char status_t;
#define STATUS_HAS_ARG       1
#define STATUS_HAS_FILE      2
#define STATUS_HAS_MEM_BLOCK 4
#define STATUS_HAS_ARG_BIT       0
#define STATUS_HAS_FILE_BIT      1
#define STATUS_HAS_MEM_BLOCK_BIT 2   
   struct grid_data_t
   {
      arg_t      arg;
      status_t   status;
      void      *address;
      rbtree    *arguments;
   };

#define grid_data_get_arguments(g)         (g->arguments) 
#define grid_data_get_argument_value(g, i) ((arg_value_t *)(*(grid_data_get_arguments(g)))[i])
#define grid_data_get_status(g)            (g->status)
#define grid_data_has_opt(g, opt_bit)      (1 & (grid_data_get_status(g) >> opt_bit))
#define grid_data_has_arguments(g)         (grid_data_has_opt(g, STATUS_HAS_ARG_BIT))
#define grid_data_has_file(g)              (grid_data_has_opt(g, STATUS_HAS_FILE_BIT))
#define grid_data_has_mem_block(g)         (grid_data_has_opt(g, STATUS_HAS_MEM_BLOCK_BIT))
   static void grid_data_set_opt(grid_data_t *data, int opt, bool on)
   {
      if(on)
	 data->status |= opt;
      else 
	 data->status &= ~opt;
   }
   static int grid_data_get_argument_count(grid_data_t *data)
   {
      if(data->arguments)
	 return data->arguments->length();
      else 
	 return 0;
   }
   static void *grid_data_get_block_addr(grid_data_t *data)
   {
      if(grid_data_has_mem_block(data))
	 return data->address;
   }
   static const char *grid_data_get_file_addr(grid_data_t *data)
   {
      if(grid_data_has_file(data))
	 return (const char *)data->address;
   }
   static void grid_data_init(grid_data_t **data)
   {
      (*data) = new grid_data_t;
      memset((*data), 0, sizeof(grid_data_t));
   }
   static void grid_data_destruct(grid_data_t *data)
   {
      if(data->arguments)
      {
	 arg_value_t *value;
	 int count = grid_data_get_argument_count(data);
	 for(int i = 1; i <= count; i++)
	 {
	    value = grid_data_get_argument_value(data, i);
	    delete value;
	 }
	 
	 delete data->arguments;
      }
   }   
};

#endif
