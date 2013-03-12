/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com>
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

#include <iostream>
#include <fstream>

#include <neweraHPC/alloc.h>

#include <include/jarvis_data.h>
#include <include/words.h>
#include <include/morphological_rules.h>

using namespace std;

namespace jarvis
{
   const char *wordnet_index_files[INDEXED_POS_COUNT];
   const char *wordnet_data_files[INDEXED_POS_COUNT];
   const char *wordnet_exc_files[INDEXED_POS_COUNT];
   
   void init_wordnet_files()
   {
      const char *index_file_name;
      const char *data_file_name;
      const char *exc_file_name;
      const char *file_suffix;
      
      for(int i = 0; i < INDEXED_POS_COUNT; i++)
      {
	 file_suffix = jv_get_pos_file_suffix(i);
	 
	 index_file_name = nhpc_strconcat(WORDNET_ROOT_DIR, "/", "index.", file_suffix);
	 data_file_name  = nhpc_strconcat(WORDNET_ROOT_DIR, "/", "data.", file_suffix);
	 exc_file_name   = nhpc_strconcat(WORDNET_ROOT_DIR, "/", "exc.", file_suffix);
	 
	 wordnet_index_files[i] = index_file_name;
	 wordnet_data_files[i]  = data_file_name;
	 wordnet_exc_files[i]   = exc_file_name;
      }
   }
   
   void init_jarvis_data()
   {
      init_jarvis_words_db();
      init_wordnet_files();
      init_morphological_rules();      
   }   
};