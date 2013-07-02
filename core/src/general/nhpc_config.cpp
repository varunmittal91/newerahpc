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

#include <stdlib.h>

#include <include/neweraHPC.h>

nhpc_pool_t *main_pool;

void nhpc_init_config(int argc, char **argv) {
   nhpc_rbtree_init((nhpc_pool_t *)main_pool, &cmdline_arguments, RBTREE_STR);
   
   char **tmp_argv = argv + 1;
   while(*tmp_argv != NULL)
   {
      char *arg = *tmp_argv;
      
      if(*arg != '-')
      {
	 LOG_ERROR("Error option should begin with a '-' sign");
	 exit(0);
      }
      
      nhpc_insert_cmdline_argument((*(tmp_argv + 1)), (arg + 1));
      
      if(*(tmp_argv + 1))
      {
	 if(*(*(tmp_argv + 1)) == '-')
	    tmp_argv += 1;
	 else 
	    tmp_argv += 2;
      }
      else 
      {
	 tmp_argv += 1;
      }
   }   
   
   const char *test_verbose = nhpc_get_cmdline_argument("v");
   if(test_verbose != NULL)
   {
      memset(log_on, false, sizeof(bool) * 3);
      
      if(nhpc_strcmp(test_verbose, "warning") == NHPC_SUCCESS)
	 log_on[Warning] = true;
      else if(nhpc_strcmp(test_verbose, "debug") == NHPC_SUCCESS)
	 log_on[Debug] = true;
      else if(nhpc_strcmp(test_verbose, "info") == NHPC_SUCCESS)
	 log_on[Info] = true;
      else if(nhpc_strcmp(test_verbose, "all") == NHPC_SUCCESS)
	 memset(log_on, true, sizeof(bool) * 3);
      else 
	 LOG_ERROR("Verbose option invalid");
   }   
}