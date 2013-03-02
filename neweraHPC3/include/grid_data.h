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

#ifndef _GRID_DATA_H_
#define _GRID_DATA_H_

/* Grid configuration file which will contain peer list */
#define grid_conf_file "/etc/grid.conf"
#define GRID_RANGE_PLUGIN "nhpc_plugin_range"

typedef unsigned char grid_arg_type;
enum GRID_ARG_TYPE
{
   RANGE,
   VALUE,
   LITERAL,
   COMMAND,
   GRID_FILE,
   MEMORY_BLOCK
};

#define ARG_RANGE        "0"
#define ARG_VALUE        "1"
#define ARG_LITERAL      "2"
#define ARG_COMMAND      "3"
#define ARG_GRID_FILE    "4"
#define ARG_MEMORY_BLOCK "5"

#define _grid_arg_type (g, t)    (1 & g >> t)
#define _grid_arg_is_range(g)    (_grid_arg_type(g, RANGE))
#define _grid_arg_is_value(g)    (_grid_arg_type(g, VALUE))
#define _grid_arg_is_literal(g)  (_grid_arg_type(g, LITERAL))
#define _grid_arg_is_command(g)  (_grid_arg_type(g, COMMAND))
#define _grid_arg_is_file(g)     (_grid_arg_type(g, GRID_FILE))
static void grid_set_arg_type(grid_arg_type *grid_arg, int arg)
{
   *grid_arg = 1;
   
   for(; arg > 0; arg--)
   {
      (*grid_arg) = (*grid_arg) << 1; 
   }
}

namespace neweraHPC
{
   struct plugin_details_t
   {
      char       *plugin_name;
      const char *plugin_dir;
      const char *path_plugin;
      const char *path_nxi;
      fnc_ptr_nhpc_plugin_t fnc_init;
      fnc_ptr_nhpc_plugin_t fnc_client_exec;
      fnc_ptr_nhpc_plugin_t fnc_exec;
      fnc_ptr_nhpc_plugin_t fnc_processor;
      fnc_ptr_nhpc_plugin_t fnc_map_reduce;
   };
   
   struct peer_details_t
   {
      char *port;
      char *host;
      int   id;
      int   threads;
      int   weight;
      int   processors;
      int   processor_time;
      bool  status;      
      bool  dynamic;
   };
   
   typedef unsigned char plugin_request_status;
   struct plugin_request_t
   {
      char  *plugin;
      char  *peer_host;
      char  *peer_port;
      plugin_request_status  status;
   };   
#define nhpc_grid_set_plugin_request_sent(p)      ((p)->status |= 1)
#define nhpc_grid_set_plugin_request_complete(p)  ((p)->status |= 2)
#define nhpc_grid_is_plugin_request_sent(p)       ((p)->status & 1)
#define nhpc_grid_is_plugin_request_complete(p)   (((p)->status >> 1) & 1)
   
   struct nhpc_result_t
   {
      void          *data;
      nhpc_size_t    content_len;
      grid_arg_type  arg_type;
   };
   
   typedef unsigned char instruction_status;
   struct nhpc_instruction_set_t
   {
      char *plugin_name;
      char *grid_uid;
      char *host_grid_uid;
      char *host_peer_addr;
      char *host_peer_port;
      int   host_peer_id;
      
      rbtree        *arguments;
      int	     argument_count;
      void          *data;
      nhpc_size_t    data_len;
      nhpc_result_t *result;

      //bool execute;
#define GRID_INSTRUCTION_STATUS_EXECUTABLE 1  
      instruction_status status;
   };
#define nhpc_grid_set_result_type(r, t) (grid_set_arg_type(&(r->arg_type), t))
#define nhpc_grid_add_result_data(i, r) (i->result = (void *)r)
   static void nhpc_grid_add_result(nhpc_instruction_set_t *instruction_set)
   {
      instruction_set->result = new nhpc_result_t;
      memset((instruction_set->result), 0, sizeof(nhpc_result_t));
   }
   static void nhpc_grid_instruction_set_opt(nhpc_instruction_set_t *instruction_set, instruction_status option, bool enable)
   {
      if(enable)
	 instruction_set->status |= option;
      else 
	 instruction_set->status &= ~option;
   }
#define nhpc_grid_instruction_erase_opt(i)           (i->status &= 0)
#define nhpc_grid_instruction_is_opt(i, o)           (i->status & o)
#define nhpc_grid_instruction_set_executable(i, on)  (nhpc_grid_instruction_set_opt(i, GRID_INSTRUCTION_STATUS_EXECUTABLE, on))
#define nhpc_grid_instruction_is_executable(i)       (nhpc_grid_instruction_is_opt(i, GRID_INSTRUCTION_STATUS_EXECUTABLE))

#define nhpc_grid_get_peer_host_instruction(i)  (i->host_peer_addr)
#define nhpc_grid_get_peer_port_instruction(i)  (i->host_peer_port)
#define nhpc_grid_get_peer_host_socket(s)       (s->host)
#define nhpc_grid_get_peer_port_socket(s)       (s->port)
};

#endif
