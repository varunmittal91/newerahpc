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
   
   char *word_dir;
   char *list_dir;

   void load_word_library(char *_word_dir)
   {
      word_dir = nhpc_strconcat(_word_dir, "/lists/");
      
      word_list = new rbtree_t(NHPC_RBTREE_STR);
      verbs = new rbtree_t(NHPC_RBTREE_STR);
      relations = new rbtree_t(NHPC_RBTREE_STR);
      adjectives = new rbtree_t(NHPC_RBTREE_STR);
      
      word_types = new rbtree_t(NHPC_RBTREE_STR);
      word_types->insert(verbs, "verbs");
      word_types->insert(relations, "relations");
      word_types->insert(adjectives, "adjectives");
      
      string_t *file_list = nhpc_get_file_list(word_dir, NHPC_VISIBLE_DIR_CHILD);
      
      for(int i = 0; i < file_list->count; i++)
      {
	 char *file_name = file_list->strings[i];	 
	 rbtree_t *main_category = (rbtree_t *)word_types->search(file_name);
	 if(!file_name)
	 {
	    LOG_ERROR("Jarvis does not know hot to handle " << file_name);
	    continue;
	 }
	 
	 char *file_path = nhpc_strconcat(word_dir, file_name);
	 
	 ifstream file_stream(file_path);
	 string line;
	 
	 getline(file_stream, line);
	 const char *line_str = line.c_str();
	 string_t *headers = nhpc_substr(line_str, ',');
	 rbtree_t **categories = new rbtree_t* [headers->count];
	 for(int i = 0; i < headers->count; i++)
	 {
	    categories[i] = new rbtree_t(NHPC_RBTREE_NUM);
	    main_category->insert((categories[i]), headers->strings[i]);
	 }
	 
	 int count_number = 0;
	 
	 while(getline(file_stream, line))
	 {
	    count_number++;
	    
	    word_meaning_t *word_meaning = new word_meaning_t;
	    
	    line_str = line.c_str();

	    string_t *word_data = nhpc_substr(line_str, ',');

	    for(int i = 0; i < word_data->count; i++)
	    {
	       string_t *words = nhpc_substr((word_data->strings[i]), '/');
	       
	       for(int j = 0; j < words->count; j++)
	       {
		  char *word_str = words->strings[j];
		  
		  list_container_t *list_container = (list_container_t *)word_list->search(word_str);
		  bool create_new = false;
		  if(!list_container)
		  {
		     list_container = new list_container_t;
		     list_container->count = 0;
		     create_new = true;
		  }
	       
		  word_t *word = new word_t;
		  nhpc_strcpy(&(word->word), word_str);
		  nhpc_strcpy(&(word->word_type), file_name);
		  nhpc_strcpy(&(word->sub_category), headers->strings[i]);
		  
		  word_t **tmp_words = new word_t* [(list_container->count) + 1];
		  tmp_words[list_container->count] = word;
		  
		  if(!create_new)
		  {
		     memcpy(tmp_words, (list_container->words), sizeof(word_t*) * (list_container->count));
		     delete[] (list_container->words);
		  }
		  else 
		  {
		     word_list->insert(list_container, word_str);
		  }
		  list_container->words = tmp_words;
		  (list_container->count)++;
		  
		  word->node_id = categories[i]->insert(word_meaning);
	       }
	    }
	    
	    nhpc_string_delete(word_data);
	 }
	 
	 LOG_INFO("Added: "<< count_number << " to: " <<file_name);

	 nhpc_string_delete(headers);
      }
   }
   
   list_container_t *search_word(const char *word)
   {
      list_container_t *list_container = (list_container_t *)word_list->search(word);
      
      return list_container;
   }
}
