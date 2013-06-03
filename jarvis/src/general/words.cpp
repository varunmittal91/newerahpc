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
#include <include/parse.h>
#include <include/jarvis_data.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   _pos_codes   pos_codes[POS_COUNT] = {1, 'r', "adv", "Adverb", 2, 'a', "adj", "Adjective", 4, 'n', "noun", "Noun", 8, 'v', "verb", "Verb" ,16, 's', NULL, "Adjective Alternate"};
   rbtree     **word_sets;
   rbtree      *relation_types;

   rbtree      *word_list_all;
   rbtree     **word_list_pos;
   nhpc_mutex_t *mutex_list_all;
   nhpc_mutex_t *mutex_list_pos;
   
   void init_jarvis_words_db()
   {
      word_sets = new rbtree* [POS_COUNT];
      for(int i = 0; i < POS_COUNT; i++)
      {
	 word_sets[i] = new rbtree;
      }
      
      relation_types = new rbtree(RBTREE_STR);
      _jv_add_relation_type(ANTONYM   , "!");
      _jv_add_relation_type(HYPERNYM  , "@");
      _jv_add_relation_type(I_HYPERNYM, "@i");
      _jv_add_relation_type(HYPONYM   , "~");
      _jv_add_relation_type(I_HYPONYM , "~i");
      _jv_add_relation_type(M_HOLONYM , "#m");
      _jv_add_relation_type(S_HOLONYM , "#s");
      _jv_add_relation_type(P_HOLONYM , "#p");
      _jv_add_relation_type(M_MERONYM , "%m");
      _jv_add_relation_type(S_MERONYM , "%s");
      _jv_add_relation_type(P_MERONYM , "%p");
      _jv_add_relation_type(ATTRIBUTE , "=");
      _jv_add_relation_type(DE_FORM   , "+");
      _jv_add_relation_type(D_TOPIC   , ";c");
      _jv_add_relation_type(M_TOPIC   , "-c");
      _jv_add_relation_type(D_REGION  , ";r");
      _jv_add_relation_type(M_REGION  , "-r");
      _jv_add_relation_type(D_USAGE   , ";u");
      _jv_add_relation_type(M_USAGE   , "-u");
      _jv_add_relation_type(ENTAILMENT, "*");
      _jv_add_relation_type(CAUSE     , ">");
      _jv_add_relation_type(A_SEE     , "^");
      _jv_add_relation_type(V_GROUP   , "$");
      _jv_add_relation_type(SIMILAR_TO, "&");
      _jv_add_relation_type(PA_VERB   , "<");
      _jv_add_relation_type(PERTAINYM , "\\");
      
      init_word_list();
   }
   
   void init_word_list()
   {
      word_list_all = new rbtree(RBTREE_STR);
      word_list_pos = new rbtree*[POS_COUNT];
      
      for(int i = 0; i < POS_COUNT; i++)
	 word_list_pos[i] = new rbtree(RBTREE_STR);
      
      mutex_list_all = new nhpc_mutex_t;
      mutex_list_pos = new nhpc_mutex_t;
      thread_mutex_init(mutex_list_all);
      thread_mutex_init(mutex_list_pos);
   }
   
   jv_pos *jv_get_word(const char *word)
   {
      thread_mutex_lock(mutex_list_all, NHPC_THREAD_LOCK_READ);
      jv_pos *_pos = _jv_get_word_list_pos(word);
      thread_mutex_unlock(mutex_list_all, NHPC_THREAD_LOCK_READ);
      
      return _pos;
   }

   word_def_t *jv_get_word(const char *word, jv_pos pos)
   {
      int pos_value = jv_get_pos_int_value(pos);
      
      thread_mutex_lock(mutex_list_all, NHPC_THREAD_LOCK_READ);
      word_def_t *word_def = (word_def_t *)(word_list_pos[pos_value]->search(word));
      thread_mutex_unlock(mutex_list_all, NHPC_THREAD_LOCK_READ);
			      
      return word_def;
   }
			      
   
   void jv_add_word(index_record_t *index_record)
   {
      const char *_word = jv_get_index_record_word(index_record);
      jv_pos      pos   = jv_get_index_record_pos(index_record);
      
      thread_mutex_lock(mutex_list_all, NHPC_THREAD_LOCK_READ);
      jv_pos *_pos = _jv_get_word_list_pos(_word);
      thread_mutex_unlock(mutex_list_all, NHPC_THREAD_LOCK_READ);
      
      word_def_t *word_def = new word_def_t;
      memset(word_def, 0, sizeof(word_def_t));

      _jv_word_def_set_pos(word_def, pos);
      _jv_word_def_set_word(word_def, _word);
      _jv_word_def_set_pointers(word_def, jv_get_index_record_pointers(index_record));
      _jv_word_def_set_offsets(word_def, jv_get_index_record_offsets(index_record));
      
      int pos_value = jv_get_pos_int_value(pos);
      
      thread_mutex_lock(mutex_list_pos, NHPC_THREAD_LOCK_WRITE);
      word_list_pos[pos_value]->insert(word_def, _word);
      thread_mutex_unlock(mutex_list_pos, NHPC_THREAD_LOCK_WRITE);
      
      if(_pos)
      {
	 jv_set_pos(*_pos, pos);
      }
      else 
      {
	 _pos = new jv_pos;
	 jv_set_pos(*_pos, pos);
	 thread_mutex_lock(mutex_list_all, NHPC_THREAD_LOCK_WRITE);
	 word_list_all->insert(_pos, _word);
	 thread_mutex_unlock(mutex_list_all, NHPC_THREAD_LOCK_WRITE);	 
      }
   }
};