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
#include <include/parse_data.h>
#include <include/morphological_rules.h>
#include <include/jarvis_data.h>
#include <include/jarvis.h>

using namespace std;

namespace jarvis
{
   void jv_get_index_record(search_param_t *search_param)
   {
      ifstream index_file(jv_get_search_param_sources(search_param));
      
      const char *word = jv_get_search_param_word(search_param);

      string      line;
      char       *line_str;
      int         compare_value;
      int         word_pos;
      const char *src_word;
      bool        found_record = false;
      
      jv_morphic_status morphic_search_status = 0;
      
   search:
      index_file.seekg(0);
      getline(index_file, line);
      while(getline(index_file, line))
      {
	 line_str = (char *)line.c_str();

	 word_pos = nhpc_strfind(line_str, ' ');
	 line_str[word_pos - 1] = '\0';
	 src_word = line_str;
	 line_str = line_str + word_pos;
	 
	 compare_value = strcmp(word, src_word);	
	 
	 if(compare_value < 0)
	    break;
	 else if(compare_value > 0)
	    continue;
	 
	 found_record = true;
	 break;
      }
      if(!found_record)
      {
	 if(jv_morphic_status_is_on(morphic_search_status))
	 {
	    word = morphological_word_search(search_param, &morphic_search_status);
	    if(word)
	       goto search;
	 }
      }
      else
      {
	 index_record_t *index_record;
	 index_record         = jv_analyze_index_record(search_param, line_str);
	 search_param->result = (void *)index_record;
	 jv_set_search_successful(search_param);
      }
      
      index_file.close();      
      jv_set_search_complete(search_param);      
   }
   
   index_record_t *jv_analyze_index_record(search_param_t *search_param, char *record_string)
   {
      char *record_part[50];
      int   count = 50;
      jv_get_record_parts(record_string, record_part, &count);

      index_record_t *index_record = new index_record_t;
      memset(index_record, 0, sizeof(index_record_t));
      
      int sense_count   = nhpc_strtoi(record_part[1]);
      int pointer_count = nhpc_strtoi(record_part[2]);
      
      jv_set_index_record_word(index_record, jv_get_search_param_word(search_param));
      jv_set_index_record_pos(index_record, jv_get_search_param_pos(search_param));
      
      for(int i = 0; i < pointer_count; i++)
	 jv_set_index_record_pointer(index_record, record_part[i + 3]);
      
      for(int i = 0; i < sense_count; i++)
	 jv_set_index_record_offset(index_record, record_part[i + 2 + pointer_count + 3]);
      
      return index_record;
   }
   
   index_record_t *jv_analyze_index_record(search_param_t *search_param ,string_t *record_parts)
   {
      /*
      int sense_count   = nhpc_strtoi(record_parts->strings[2]);
      int pointer_count = nhpc_strtoi(record_parts->strings[3]);
      
      index_record_t *index_record = new index_record_t;
      memset(index_record, 0, sizeof(index_record_t));
      
      index_record->data_offsets   = new rbtree(RBTREE_NUM);
      index_record->pointers       = new rbtree(RBTREE_NUM);

      jv_set_index_record_word(index_record, jv_get_search_param_word(search_param));
      jv_set_index_record_pos(index_record, jv_get_search_param_pos(search_param));

      for(int i = 0; i < pointer_count; i++)
	 jv_set_index_record_pointer(index_record, record_parts->strings[i + 4]);
      
      for(int i = 0; i < sense_count; i++)
      {
	 jv_set_index_record_offset(index_record, record_parts->strings[i + 3 + pointer_count + 3]);
      }
      
      return index_record;
       */
   }
   
   void jv_join_threads_search_params(search_param_t *search_params)
   {
      for(int i = 0; i < INDEXED_POS_COUNT; i++)
      {
	 (*thread_manager).join_thread(jv_get_search_param_thread_id(&(search_params[i])));
      }
   }
   
   nhpc_status_t jv_get_word_def(const char *word)
   {
      if(jv_get_word(word))
	 return NHPC_SUCCESS;
      
      search_param_t  search_params[INDEXED_POS_COUNT];
      search_param_t *search_param;
      
      jv_pos      pos;
      const char *source_file;
      
      int thread_id;
      for(int i = 0; i < INDEXED_POS_COUNT; i++)
      {
	 search_param = &(search_params[i]);
	 memset(search_param, 0, sizeof(search_param_t));
	 
	 pos         = jv_get_pos_int_code(i);
	 source_file = wordnet_index_files[i];

	 jv_set_search_param_pos(search_param, pos);
	 jv_set_search_param_source(search_param, source_file);
	 jv_set_search_param_word(search_param, word);
	 
	 (*thread_manager).init_thread(&thread_id, NULL);
	 (*thread_manager).create_thread(&thread_id, NULL, (void* (*)(void*))jv_get_index_record, search_param, NHPC_THREAD_DEFAULT);
	 
	 jv_set_search_param_thread_id(search_param, thread_id);
      }
      
      jv_join_threads_search_params(search_params);
      
      nhpc_status_t nrv = NHPC_FAIL;
      
      for(int i = 0; i < INDEXED_POS_COUNT; i++)
      {
	 search_param = &(search_params[i]);
	 (*thread_manager).delete_thread_data(jv_get_search_param_thread_id(search_param));
	 
	 if(jv_search_is_complete(search_param))
	 {
	    if(jv_search_is_successful(search_param))
	    {
	       nrv = NHPC_SUCCESS;
	       
	       search_param_t *data_search;
	       search_param_init(&data_search);
	       
	       pos         = jv_get_pos_int_code(i);
	       source_file = wordnet_data_files[i];
	       LOG_INFO("Searching for data in :" << source_file);
	       
	       jv_set_search_param_pos(data_search, pos);
	       jv_set_search_param_source(data_search, source_file);
	       jv_set_search_param_word(data_search, word);
	       jv_set_search_param_data(data_search, search_param);
	       	       
	       jv_get_word_data_record(data_search);
	       jv_add_word((index_record_t *)search_param->result);
	       
	       search_param_destruct(data_search);
	       delete data_search;
	    }
	 }
      }
      LOG_INFO("Threads done");
      
      return nrv;
   }
};

