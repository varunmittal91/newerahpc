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

#ifndef _NEWERAHPC_H_
#define _NEWERAHPC_H_

#include "file.h"
#include "rbtree.h"
#include "list.h"
#include "thread.h"
#include "network.h"
#include "strings.h"
#include "general.h"
#include "system.h"
#include "alloc.h"
#include "http.h"
#include "json.h"

#include <iostream>

#define nhpc_get_cmdline_argument(v)           ((const char *)(*cmdline_arguments).search(v));
#define nhpc_get_cmdline_argument_key(i, key)  ((const char *)(*cmdline_arguments).search_inorder_str(i, &key));
#define nhpc_get_cmdline_argument_count()      ((*cmdline_arguments).length());
#define nhpc_insert_cmdline_argument(v, key)   (*cmdline_arguments).insert(v, key);
#define nhpc_delete_cmdline_argument(v)        (*cmdline_arguments).erase(v);
extern neweraHPC::rbtree *cmdline_arguments;

using namespace std;

namespace neweraHPC
{
   void neweraHPC_init(int argc, char **argv);
   void neweraHPC_destruct();

   void exit_handler(int signal);
};

#endif