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

#ifndef _JARVIS_DATA_H_
#define _JARVIS_DATA_H_

#include <neweraHPC/rbtree.h>
#include <neweraHPC/thread.h>
#include <neweraHPC/error.h>

#include "words.h"

using namespace neweraHPC;

namespace jarvis
{
   class jarvis_data_t
   {
   private:
      rbtree_t *jarvis_dictionary;
      rbtree_t *jarvis_dictionary_pos;
      rbtree_t *failed_searches;
      rbtree_t *morphological_database;
      
      nhpc_mutex_t *mutex_ptr_symbols;
      rbtree_t *pointer_database; // contains all pointer symbol strings and corresponding enum identifier
      
      nhpc_mutex_t *mutex;
      
      char **exception_files;
      
      struct morphological_pair_t
      {
	 const char *suffix;
	 const char *ending;
      };
   public:
      jarvis_data_t();
      ~jarvis_data_t();
      void init();
      void init_morphological_database();

      ptr_symbol_set_t *search_ptr_symbol(char *symbol);
      
      nhpc_status_t add_word(index_record_t *index_record);

      rbtree_t *lookup_word(const char *word);
      index_record_t *lookup_word(const char *word, int pos);
      
      char *morphological_analyses(char *word, int pos);
      
      nhpc_status_t load_word(const char *word, int pos = NULL);
      //nhpc_status_t load_word(const char *word);
      
      nhpc_status_t read_index_record(index_record_t *index_record);
   };
   
   extern jarvis_data_t jarvis_data;
};

#endif