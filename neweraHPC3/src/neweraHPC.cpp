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

#include <iostream>
#include <stdlib.h>
#include <signal.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/neweraHPC.h>
#include <include/error.h>
#include <include/strings_pool.h>

using namespace std;

neweraHPC::rbtree *cmdline_arguments;

namespace neweraHPC
{
   void exit_handler(int signal)
   {
      exit(0);
   }
   
   void neweraHPC_init(int argc, char **argv)
   {      
      signal(SIGPIPE, SIG_IGN);
      signal(SIGINT, exit_handler);
      
      cmdline_arguments = new rbtree(RBTREE_STR);
      
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
	    log_on[Log::Warning] = true;
	 else if(nhpc_strcmp(test_verbose, "debug") == NHPC_SUCCESS)
	    log_on[Log::Debug] = true;
	 else if(nhpc_strcmp(test_verbose, "info") == NHPC_SUCCESS)
	    log_on[Log::Info] = true;
	 else if(nhpc_strcmp(test_verbose, "all") == NHPC_SUCCESS)
	    memset(log_on, true, sizeof(bool) * 3);
	 else 
	    LOG_ERROR("Verbose option invalid");
	 
	 nhpc_delete_cmdline_argument("v");
      }
   }
   
   void neweraHPC_destruct()
   {
      delete cmdline_arguments;
   }
};
