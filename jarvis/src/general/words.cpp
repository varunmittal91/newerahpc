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
#include <include/words.h>

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
   
   char *word_dir;

   void load_word_library(char *_word_dir)
   {
      word_dir = nhpc_strconcat(_word_dir, "/words/");

      word_list = new rbtree_t(NHPC_RBTREE_STR);
      word_types = new rbtree_t(NHPC_RBTREE_STR);
      
      relations = new rbtree_t(NHPC_RBTREE_STR);
      nodes = new rbtree_t(NHPC_RBTREE_STR);
      functions = new rbtree_t(NHPC_RBTREE_STR);
      verbs = new rbtree_t(NHPC_RBTREE_STR);
      
      word_types->insert(relations, "relation");
      word_types->insert(nodes, "node");
      word_types->insert(functions, "function");
      word_types->insert(verbs, "verb");

      string_t *string = nhpc_get_file_list(word_dir, NHPC_VISIBLE_DIR_CHILD);
      if(string == NULL)
      {
         LOG_ERROR("JARVIS cannot read data");
         exit(1);
      }

      for(int i = 0; (string->strings[i]); i++)
      {
	 char *word_file_name = string->strings[i];
	 
         char *tmp_word_file = nhpc_strconcat(word_dir, string->strings[i]);
	 
	 string_t *tmp_word = nhpc_substr(string->strings[i], ':');
	 char *word_type = tmp_word->strings[0];
	 char *word      = tmp_word->strings[1];
	 
	 cout<<word<<" "<<word_type<<endl;
	 
	 rbtree_t *tmp_rbtree = (rbtree_t *)word_types->search(word_type);
	 if(tmp_rbtree == NULL)
	 {
	    LOG_ERROR("JARVIS does not know how to handle " << string->strings[i] << " from file " << string->strings[i]);
	 }
	 else 
	 {
	    word_definition_t *word_definition = (word_definition_t *)word_list->search(word);
	    
	    if(!word_definition)
	    {
	       word_definition = new word_definition_t;
	       word_definition->word_files = new char*;
	       word_definition->word_types = new char*;
	       word_definition->count = 0;
	       
	       word_list->insert(word_definition, word);
	    }
	    else 
	    {
	       char **tmp_word_files = new char* [word_definition->count + 1];
	       char **tmp_word_types = new char* [word_definition->count + 1];
	       
	       memcpy(tmp_word_files, (word_definition->word_files), sizeof(char **) * (word_definition->count));
	       memcpy(tmp_word_types, (word_definition->word_types), sizeof(char **) * (word_definition->count));
	       
	       delete[] (word_definition->word_types);
	       delete[] (word_definition->word_files);
	       
	       word_definition->word_files = tmp_word_files;
	       word_definition->word_types = tmp_word_types;
	    }
	    
	    int *count = &(word_definition->count);
	    
	    nhpc_strcpy(&(word_definition->word_files[*count]), tmp_word_file);
	    nhpc_strcpy(&(word_definition->word_types[*count]), word_type); 
	    
	    (*count)++;
	    
	    tmp_rbtree->insert(word_definition, word);

	    nhpc_string_delete(tmp_word);
	 }
      }
      
      nhpc_string_delete(string);
   }
}
