#ifndef _JARVIS_DATA_H_
#define _JARVIS_DATA_H_

#include <neweraHPC/rbtree.h>
#include <neweraHPC/thread.h>

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
      
      nhpc_status_t add_word(index_record_t *index_record);
      rbtree_t *lookup_word(const char *word);
      index_record_t *lookup_word(const char *word, int pos);
      char *morphological_analyses(char *word, int pos);
      
      nhpc_status_t load_word(const char *word);
   };
   
   extern jarvis_data_t jarvis_data;
};

#endif