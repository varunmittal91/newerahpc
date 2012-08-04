#ifndef _JARVIS_WORDS_H_
#define _JARVIS_WORDS_H_

#include <neweraHPC/rbtree.h>

using namespace neweraHPC;

namespace jarvis
{
   struct word_definition_t
   {
      int count;
      char **word_files;
      char **word_types;
   };
   
   struct verb_t
   {
      char *base_form;
      char *past_simple;
      char *past_particilple;
      char *present_participle;
      char *third_person_singular;
   };
   
   struct word_t
   {
      char *word_type;
      char *word;
   };
   
   struct word_meaning_t
   {
      
   };
   
   extern rbtree_t *word_list;
   extern rbtree_t *word_types;
   extern rbtree_t *relations;
   extern rbtree_t *nodes;
   extern rbtree_t *functions;
   
   extern rbtree_t *verbs;
   
   extern char *word_dir;
   
   void load_word_library(char *_word_dir);
   
   nhpc_status_t search_word(const char *word, word_definition_t *word_definition);
   nhpc_status_t search_word_context(const char *word, word_definition_t *word_definition, int *index);
};

#endif
