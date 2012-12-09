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

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <neweraHPC/neweraHPC.h>
#include <neweraHPC/list.h>

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
   const char *word_net_datas[INDEX_FILES_COUNT] = {"data.adv", "data.adj", "data.noun", "data.verb"};
   const char **word_net_index_files;
   const char **word_net_data_files;
   
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
      word_net_data_files = new const char* [INDEX_FILES_COUNT];
      for(int i = 0; i < INDEX_FILES_COUNT; i++)
      {
	 word_net_index_files[i] = nhpc_strconcat(word_net_dir, "/", word_net_indexs[i]);
	 word_net_data_files[i] = nhpc_strconcat(word_net_dir, "/", word_net_datas[i]);
	 cout << "loading index file: " << word_net_index_files[i] << endl;
      }
      
      word_list = new rbtree_t(NHPC_RBTREE_STR);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t word_lookup(const char *_word, word_record_t **word_record_ptr)
   {
      rbtree_t *word_tree = jarvis_data.lookup_word(_word);      
   }
   
   rbtree_t *read_data_file(const char *data_file_name, list_t *synset_offsets, const char *word, int ptr_symbol = NULL)
   {
      LOG_INFO("Reading data file for word: " << word << " in: " << data_file_name);
      
      //index_record_t *index_record = search_param->index_record;
      //index_record->synset_subsets = new rbtree_t(NHPC_RBTREE_NUM_HASH);
      //rbtree_t *synset_subsets = index_record->synset_subsets;
      
      ifstream data_file(data_file_name);
      if(!data_file.is_open())
      {
	 return NULL;
      }
      
      int offset;
      string line;
      
      rbtree_t *word_senses = new rbtree_t;
      rbtree_t *word_tree;      
      list_t *synset_offsets_tmp = new list_t(LIST_MIN_FIRST);
      
      //while((offset = (synset_offsets->pop_elem())) != -1)
      while(1)
      {
	 offset = synset_offsets_tmp->pop_elem();	 
	 if(offset == -1)
	 {
	    offset = synset_offsets->pop_elem();
	    if(offset == -1)
	    {
	       break;
	    }
	    else 
	    {
	       cout << "extracting new sense" << endl;
	       
	       word_tree = new rbtree_t;
	       word_senses->insert(word_tree);
	    }
	 }
	 
	 rbtree_t *word_set = new rbtree_t();
	 word_tree->insert(word_set);
	 
	 data_file.seekg(offset);
	 getline(data_file, line);
	 
	 const char *line_str = line.c_str();
	 
	 int pos = nhpc_strfind(line_str, '|');
	 char *synset_str = nhpc_substr(line_str, 1, pos - 1);
	 
	 string_t *synset_parts = nhpc_substr(synset_str, ' ');
	 char *word_count_str = synset_parts->strings[3];
	 int word_count = nhpc_strtoi(word_count_str);
	 //index_record->synset_words = new index_record_t* [word_count];
	 
	 char *ptr_count_str = synset_parts->strings[4 + 2 * word_count];
	 int ptr_count = nhpc_strtoi(ptr_count_str);
	 
	 int part_of_speech = (int)*(synset_parts->strings[2]);
	 
	 int parts_position = 4;
	 
	 LOG_INFO("Reading new sense");
	 LOG_INFO("Synset word count: " << word_count);
	 
	 for(int i = 0; i < word_count; i++)
	 {
	    char *synset_word = synset_parts->strings[parts_position];

	    parts_position += 2;
	    
	    LOG_INFO("Word in synset: " << synset_word);

	    word_set->insert(synset_word);
	    
	    //cout << "looking word: " << synset_word << endl;
	    //jarvis_data.lookup_word(synset_word);
	    //cout << "done" << endl;
	    
	    /*
	    index_record_t *new_index_record = new index_record_t;
	    nhpc_strcpy((char **)&(new_index_record->lemma), synset_word);
	    new_index_record->pos = part_of_speech;
	    
	    jarvis_data.add_word(new_index_record);

	    index_record->synset_words[i] = new_index_record;
	     */
	 }
	 
	 parts_position = 4 + 2 * word_count + 1;
	 for(int i = 0; i < ptr_count; i++)
	 {
	    char *ptr_symbol_str = synset_parts->strings[parts_position];
	    ptr_symbol_set_t *ptr_symbol_set = jarvis_data.search_ptr_symbol(ptr_symbol_str);
	    if(!ptr_symbol_set)
	    {
	       //LOG_ERROR("Pointer symbol: " << ptr_symbol_str << " does not exist");
	       
	       continue;
	    }

	    if(ptr_symbol)
	    {
	       if(ptr_symbol == ptr_symbol_set->ptr_symbol_num)
	       {
		  (*synset_offsets_tmp).add_elem(nhpc_strtoi(synset_parts->strings[parts_position + 1]));
		  
		  break;
	       }
	    }
	    if(!ptr_symbol)
	    {
	       //cout << "no specifi pointer asked" << endl;
	    }

	    parts_position += 4;
	 }
	 
	 //exit(0);
      }
      
      data_file.close();
      
      int word_sense_count = word_senses->ret_count();
      for(int i = 1; i <= word_sense_count; i++)
      {
	 cout << "sense:" << word_sense_count << endl;
	 
	 rbtree_t *word_tree = (rbtree_t *)(*word_senses)[i];
	 
	 int synset_level_count = word_tree->ret_count();
	 
	 for(int j = 1; j <= synset_level_count; j++)
	 {
	    for(int x = 1; x < j; x++)
	       cout << "\t";
	    
	    rbtree_t *words = (rbtree_t *)(*word_tree)[j];
	    
	    int word_count = words->ret_count();
	    
	    for(int k = 1; k <= word_count; k++)
	    {
	       char *word = (char *)(*words)[k];
	       
	       cout << word << ",";
	    }
	    
	    cout << endl;
	 }
      }
      
      
      LOG_INFO("Data read cycle complete");
      
      return word_senses;
   }
   
   void *read_index_file(search_param_t *search_param)
   {
      LOG_INFO("Reading index file for word: " << search_param->word << " in: " << search_param->file_name);

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
	       index_record->synset_offsets = new list_t(LIST_MIN_FIRST);
	       search_param->index_record = index_record;
	       
	       string_t *index_parts = nhpc_substr(line_str, ' ');
	       index_record->pos = *(index_parts->strings[1]);
	       nhpc_strcpy((char **)&(index_record->lemma), (index_parts->strings[0]));
	       
	       int *synset_cnt = &(index_record->synset_cnt);
	       int *symbols_cnt = &(index_record->symbols_cnt);
	       (*synset_cnt) = nhpc_strtoi(index_parts->strings[2]);
	       (*symbols_cnt) = nhpc_strtoi(index_parts->strings[3]);
	       
	       int field_count = (*synset_cnt) + (*symbols_cnt) + 6;
	       if(field_count != index_parts->count)
	       {
		  cout << "white space symbol found in index file" << endl;
		  exit(0);
	       }
	       
	       index_record->symbols_ptr = new short int[*symbols_cnt];
	       
	       for(int i = 4; i < (4 + *symbols_cnt); i++)
	       {
		  ptr_symbol_set_t *ptr_symbol_set = (ptr_symbol_set_t *)jarvis_data.search_ptr_symbol(index_parts->strings[i]);
		  
		  if(!ptr_symbol_set)
		  {
		     continue;
		  }

		  index_record->symbols_ptr[i - 4] = ptr_symbol_set->ptr_symbol_num;
	       }
	       
	       list_t *synset_offsets = (index_record->synset_offsets);
	       
	       for(int i = (4 + *symbols_cnt + 2); i < field_count; i++)
	       {
		  (*synset_offsets).add_elem(nhpc_strtoi(index_parts->strings[i]));
	       }
	       
	       jarvis_data.add_word(search_param->index_record);
	       //read_data_file(search_param, &(synset_offsets), HYPERNYM);
	       search_param->index_record->is_done = true;
				 
	       found_word = true;
	       break;
	    }
	 }
	 
	 line_number++;
      }
      
      if(found_word)
      {
	 cout << "found word: " << search_param->word << " in: " << search_param->file_name << endl << endl;
      }
      
      index_file.close();
      
      search_param->is_complete = true;
      
      LOG_INFO("Index read complete");
   }
};