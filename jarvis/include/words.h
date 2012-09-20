#ifndef _JARVIS_WORDS_H_
#define _JARVIS_WORDS_H_

#include <neweraHPC/rbtree.h>
#include <neweraHPC/thread.h>

#include "jarvis.h"

#define INDEX_FILES_COUNT 4
#define POS_COUNT 5

using namespace neweraHPC;

namespace jarvis
{   
   extern rbtree_t *word_list;
   extern rbtree_t *word_types;
   extern rbtree_t *relations;
   extern rbtree_t *nodes;
   extern rbtree_t *functions;
   extern rbtree_t *adjectives;
   extern rbtree_t *verbs;
   
   /*based on characters defined in WordNet */
   enum POS 
   {
      ADV = 114, //r
      ADJ = 97, //a
      ADJ_SAT = 115, //s
      NOUN = 110, //n
      VERB = 118 //v
   };
   
   extern const char *word_net_indexs[4];
   extern const char **word_net_index_files;
   
   struct index_record_t
   {
      const char *lemma;
      char pos;
      int synset_cnt;
      int p_cnt;
      int tagsense_cnt;
      int *synset_offset;
   };
   
   struct failed_record_t
   {
   };
   
   struct search_param_t
   {
      const char *word;
      const char *file_name;
      index_record_t *index_record;
      bool is_complete;
   };
   
   struct word_record_t
   {
      bool has_word;
      bool has_verb;
      bool has_noun;
      bool has_adj;
      bool has_adv;
      char *word;
      index_record_t *index_record;
   };
   
   extern const char *word_dir;
   extern const char *word_net_dir;
   
   nhpc_status_t init_word_net_database();
   nhpc_status_t word_lookup(const char *_word, word_record_t **word_record_ptr);
   void *read_index_file(search_param_t *search_param);
};

#endif
