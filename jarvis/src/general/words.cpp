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
#include <fstream>
#include <string>
#include <neweraHPC/neweraHPC.h>

#include <include/general.h>
#include <include/words.h>
#include <include/jarvis_data.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   rbtree_t *word_list;
   rbtree_t *word_types;
   rbtree_t *relations;
   rbtree_t *nodes;
   rbtree_t *functions;
   rbtree_t *verbs;
   rbtree_t *adjectives;
   
   const char *word_dir;
   const char *word_net_dir;  
   const char *word_net_indexs[INDEX_FILES_COUNT] = {"index.adv", "index.adj", "index.noun", "index.verb"};
   const char **word_net_index_files;
   
   nhpc_status_t init_word_net_database()
   {
      nhpc_status_t nrv;
      
      word_dir = (const char *)cmdline_arguments.search("d");
      
      if(word_dir == NULL)
	 word_dir = "data";
      
      nrv = nhpc_fileordirectory(word_dir);
      if(nrv != NHPC_DIRECTORY)
      {
	 LOG_ERROR("Jarvis Data Directory Missing");
	 return NHPC_FAIL;
      }
      
      word_net_dir = nhpc_strconcat(word_dir, "/", "dict");
      if(nrv != NHPC_DIRECTORY)
      {
	 LOG_ERROR("WordNET Database Missing");
	 return NHPC_FAIL;
      }
      
      word_net_index_files = new const char* [INDEX_FILES_COUNT];
      for(int i = 0; i < INDEX_FILES_COUNT; i++)
      {
	 word_net_index_files[i] = nhpc_strconcat(word_net_dir, "/", word_net_indexs[i]);
	 cout << "loading index file: " << word_net_index_files[i] << endl;
      }
      
      word_list = new rbtree_t(NHPC_RBTREE_STR);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t word_lookup(const char *_word, word_record_t **word_record_ptr)
   {
      rbtree_t *word_tree = jarvis_data.lookup_word(_word);      
   }
   
   void *read_index_file(search_param_t *search_param)
   {
      bool found_word = false;
      
      search_param->index_record = NULL;
      
      string line;
      ifstream index_file(search_param->file_name);
      int line_number = 1;
      
      while(getline(index_file, line))
      {
	 const char *line_str = line.c_str();
	 
	 int space = nhpc_strfind(line_str, ' ');
	 
	 char *src_word = nhpc_substr(line_str, 1, space - 1);
	 int cmpr_status;
	 
	 if(src_word != NULL)
	 {
	    cmpr_status = strcmp((search_param->word), src_word);
	    
	    delete[] src_word;
	    
	    if(cmpr_status < 0)
	       break;
	    else if(cmpr_status == 0)
	    {
	       index_record_t *index_record = new index_record_t;
	       search_param->index_record = index_record;
	       
	       string_t *index_parts = nhpc_substr(line_str, ' ');
	       index_record->pos = *(index_parts->strings[1]);
	       nhpc_strcpy((char **)&(index_record->lemma), (index_parts->strings[0]));
	       
	       found_word = true;
	       break;
	    }
	 }
	 
	 line_number++;
      }
      
      if(found_word)
      {
	 cout << "found word: " << search_param->word << " in: " << search_param->file_name << endl;
      }
      
      index_file.close();
      
      search_param->is_complete = true;
   }
};