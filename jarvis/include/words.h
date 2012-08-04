#ifndef _JARVIS_WORDS_H_
#define _JARVIS_WORDS_H_

#include <neweraHPC/rbtree.h>

using namespace neweraHPC;

namespace jarvis
{
   struct word_type_definition_t
   {
      char **word_features;
      int count;
      rbtree_t *data;
   };
   
   struct word_t
   {
      char *word_type;
      char *word;
      char *sub_category;
      int node_id;
   };
   
   struct list_container_t 
   {
      word_t **words;
      int count;
   };
   
   struct word_meaning_t
   {
      
   };
   
   extern rbtree_t *word_list;
   extern rbtree_t *word_types;
   extern rbtree_t *relations;
   extern rbtree_t *nodes;
   extern rbtree_t *functions;
   extern rbtree_t *adjectives;
   
   extern rbtree_t *verbs;
   
   extern char *word_dir;
   
   void load_word_library(char *_word_dir);
   
   list_container_t *search_word(const char *word);
};

#endif
