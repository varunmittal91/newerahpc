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

#include <fstream>
#include <string>
#include <iostream>

#include <neweraHPC/error.h>
#include <neweraHPC/list.h>

#include <include/parse_index.h>
#include <include/parse_data.h>
#include <include/words.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   void jv_get_word_data_record(search_param_t *search_param)
   {
      ifstream record_file(jv_get_search_param_sources(search_param));
      if(!record_file.is_open())
      {
	 jv_set_search_complete(search_param);
	 return;
      }
      
      index_record_t *index_record = _jv_get_search_param_index_record(search_param);
      int _part_of_speech = jv_get_pos_int_value(jv_get_search_param_pos(search_param)); 
      
      string  line;
      int     offset;
      int     i = 1;
      list_t *synset_offsets_tmp = new list_t(LIST_MIN_FIRST);      
      
      while(jv_get_index_record_offset(index_record, i, offset))
      {
	 (*synset_offsets_tmp).add_elem(offset);
	 i++;
      }

      while(1)
      {
	 offset = (*synset_offsets_tmp).pop_elem();
	 if(offset == -1)
	 {
	    break;
	 }
	 if(jv_get_word_set_db(_part_of_speech, offset))
	 {
	    continue;
	 }
	 
	 synset_t *synset;
	 jv_init_synset(&synset);
	 
	 record_file.seekg(0, record_file.beg);
	 record_file.seekg(offset);
	 getline(record_file, line);
	 
	 const char *line_str = line.c_str();
	 
	 int pos = nhpc_strfind(line_str, '|');
         char *synset_str = nhpc_substr(line_str, 1, pos - 1);
         if(!synset_str)
	 {
	    cout << line << endl;
	    continue;
	 }
	 
         string_t *synset_parts = nhpc_substr(synset_str, ' ');
         char *word_count_str = synset_parts->strings[3];
         
	 int word_count = nhpc_hexstrtoi(word_count_str);
	 if(word_count > 50)
	    cout << line << endl;
         
         char *ptr_count_str = synset_parts->strings[4 + 2 * word_count];
         int ptr_count = nhpc_strtoi(ptr_count_str);
         
         int part_of_speech = (int)*(synset_parts->strings[2]);
         
         int parts_position = 4;

	 cout << "New synset:";
	 for(int i = 0; i < word_count; i++)
         {
            char *synset_word;
            nhpc_strcpy(&synset_word, synset_parts->strings[parts_position]);
            parts_position += 2;
	    jv_set_synset_word(synset, synset_word);
	    
	    cout << synset_word << " ";
         }
	 cout << endl;
	 parts_position = 4 + 2 * word_count + 1;
	 
	 for(int i = 0; i < ptr_count; i++)
         {
	    const char *ptr_symbol_str = synset_parts->strings[parts_position];
	    int relation_type = _jv_get_relation_type(ptr_symbol_str);

	    if(relation_type != HYPERNYM && relation_type != I_HYPERNYM)
	    {
	       parts_position += 4;
	       continue;
	    }
	     
	    int new_offset = nhpc_strtoi(synset_parts->strings[parts_position + 1]);
	    const char *new_pos = synset_parts->strings[parts_position + 2];
	    
	    if(jv_get_pos_ascii_code(_part_of_speech) == *new_pos)
	    {
	       int *_new_offset = new int;
	       *_new_offset     = new_offset;
	       
	       synset->relations->insert(_new_offset, relation_type);
	       if(!(jv_get_word_set_db(_part_of_speech, new_offset)))
	       {
		  (*synset_offsets_tmp).add_elem(new_offset);
	       }
	    }
	    
	    parts_position += 4;
         }
	 
	 jv_insert_word_set_db(_part_of_speech, synset, offset);
	 nhpc_string_delete(synset_parts);
      }
      
      delete synset_offsets_tmp;
      
      jv_set_search_complete(search_param);
      jv_set_search_successful(search_param);
   }
};