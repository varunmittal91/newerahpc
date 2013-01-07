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
#include <string>

#include <neweraHPC/strings.h>

#include <include/parse_index.h>
#include <include/jarvis_data.h>
#include <include/jarvis.h>

using namespace std;

namespace jarvis
{
   void jv_get_index_record(search_param_t *search_param)
   {
      ifstream index_file(jv_get_search_param_sources(search_param));
      
      string      line;
      const char *line_str;
      string_t   *record_parts;
      int         compare_value;
      
      while(getline(index_file, line))
      {
	 line_str = line.c_str();

	 record_parts = nhpc_substr(line_str, ' ');

	 const char *src_word = record_parts->strings[0];
	 compare_value = strcmp(jv_get_search_param_word(search_param), src_word);
	 if(compare_value == 0)
	    break;
	 else 
	 {
	    nhpc_string_delete(record_parts);
	    record_parts = NULL;
	    
	    if(compare_value < 0)
	       break;
	 }
      }
      index_file.close();      
      jv_set_search_complete(search_param);
      
      if(!record_parts)
	 return;
      
      index_record_t *index_record = jv_analyze_index_record(search_param, record_parts);
      search_param->result = (void *)index_record;
      
      nhpc_string_delete(record_parts);
   }
   
   index_record_t *jv_analyze_index_record(search_param_t *search_param ,string_t *record_parts)
   {
      int sense_count   = nhpc_strtoi(record_parts->strings[2]);
      int pointer_count = nhpc_strtoi(record_parts->strings[3]);
      
      index_record_t *index_record = new index_record_t;
      index_record->data_offsets   = new list_t(LIST_MIN_FIRST);
      index_record->pointers       = new rbtree(RBTREE_NUM);

      jv_set_index_record_word(index_record, jv_get_search_param_word(search_param));
      jv_set_index_record_pos(index_record, jv_get_search_param_pos(search_param));

      for(int i = 0; i < pointer_count; i++)
	 jv_set_index_record_pointer(index_record, record_parts->strings[i + 4]);
      
      for(int i = 0; i < sense_count; i++)
	 jv_set_index_record_offset(index_record, record_parts->strings[i + 3 + pointer_count + 3]);
      
      return index_record;
   }
   
   void jv_join_threads_search_params(search_param_t *search_params)
   {
      for(int i = 0; i < INDEXED_POS_COUNT; i++)
      {
	 (*thread_manager).join_thread(jv_get_search_param_thread_id(&(search_params[i])));
      }
   }
   
   void jv_get_word_def(const char *word)
   {
      search_param_t  search_params[INDEXED_POS_COUNT];
      search_param_t *search_param;
      
      jv_pos      pos;
      const char *source_file;
      
      int thread_id;
      for(int i = 0; i < INDEXED_POS_COUNT; i++)
      {
	 search_param = &(search_params[i]);
	 memset(search_param, 0, sizeof(search_param_t));
	 
	 pos         = jv_get_pos_int_value(i);
	 source_file = wordnet_index_files[i];

	 jv_set_search_param_pos(search_param, pos);
	 jv_set_search_param_source(search_param, source_file);
	 jv_set_search_param_word(search_param, word);
	 
	 (*thread_manager).init_thread(&thread_id, NULL);
	 (*thread_manager).create_thread(&thread_id, NULL, (void* (*)(void*))jv_get_index_record, search_param, NHPC_THREAD_DEFAULT);
	 
	 jv_set_search_param_thread_id(search_param, thread_id);
      }
      
      jv_join_threads_search_params(search_params);
      
      for(int i = 0; i < INDEXED_POS_COUNT; i++)
      {
	 search_param = &(search_params[i]);
	 
	 if(jv_search_is_complete(search_param))
	 {
	    cout << i << " Complete" << endl;
	 }
      }
      cout << "Threads done" << endl;
   }
};

