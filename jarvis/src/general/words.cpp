/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com> & Uday Vir Singh <udayvirsingh@gmail.com>
 *	jarvis program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of jarvis.
 *
 *	jarvis is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	jarvis is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with jarvis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <neweraHPC/neweraHPC.h>

#include <include/general.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   rbtree_t *word_list;
   char *word_dir;

   void load_word_library(char *_word_dir)
   {
      word_dir = nhpc_strconcat(_word_dir, "/words/");

      word_list = new rbtree_t(NHPC_RBTREE_STR);

      string_t *string = nhpc_get_file_list(word_dir, NHPC_VISIBLE_DIR_CHILD);
      if(string == NULL)
      {
         LOG_ERROR("JARVIS cannot read data");
         exit(1);
      }

      for(int i = 0; (string->strings[i]); i++)
      {
         char *tmp_word_file = nhpc_strconcat(word_dir, string->strings[i]);
         word_list->insert(tmp_word_file, string->strings[i]);
      }

      nhpc_string_delete(string);
   }
}
