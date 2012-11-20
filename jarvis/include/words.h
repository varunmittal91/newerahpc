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

#ifndef _JARVIS_WORDS_H_
#define _JARVIS_WORDS_H_

#include <neweraHPC/rbtree.h>
#include <neweraHPC/thread.h>
#include <neweraHPC/list.h>

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
   
   extern const char *word_net_indexs[INDEX_FILES_COUNT];
   extern const char *word_net_datas[INDEX_FILES_COUNT];
   
   /*based on characters defined in WordNet */
   enum POS 
   {
      ADV = 114, //r
      ADJ = 97, //a
      ADJ_SAT = 115, //s
      NOUN = 110, //n
      VERB = 118 //v
   };
   
   struct ptr_symbol_set_t
   {
      int ptr_symbol_num;
      char *ptr_symbol_str;
   };
   
   enum ptr_symbol
   {
      ANTONYM,       // !  / Adv, Adj, Verb, Noun / Antonym 
      HYPERNYM,      // @  / Verb, Noun           / Hypernym
      I_HYPERNYM,    // @! / Noun                 / Instance Hypernym
      HYPONYM,       //    / Verb, Noun           / Hyponym
      I_HYPONYM,     // i  / Noun                 / Instance Hyponym
      M_HOLONYM,     // #m / Noun                 / Member holonym
      S_HOLONYM,     // #s / Noun                 / Sybstane holonym
      P_HOLONYM,     // #p / Noun                 / Part holonym
      M_MERONYM,     // %m / Noun                 / Member meronym
      S_MERONYM,     // %s / Noun                 / Substance meronym
      P_MERONYM,     // %p / Noun                 / Part meronym
      ATTRIBUTE,     // =  / Adj, Noun            / Attribute
      DE_FORM,       // +  / Verb, Noun           / Derivationally related form
      D_TOPIC,       // ;c / Adv, Adj, Verb, Noun / Domain of synset - TOPIC
      M_TOPIC,       // -c / Noun                 / Member of this domain - TOPIC
      D_REGION,      // ;r / Adv, Adj, Verb, Noun / Domain of synset - REGION 
      M_REGION,      // -r / Noun                 / Member of this domain - REGION 
      D_USAGE,       // ;u / Adv, Adj, Verb, Noun / Domain of synset - USAGE
      M_USAGE,       // -u / Noun                 / Member of this domain - USAGE
      ENTAILMENT,    // *  / Verb                 / Entailment
      CAUSE,         // >  / Verb                 / Cause
      A_SEE,         // ^  / Adj, Verb            / Also see
      V_GROUP,       // $  / Verb                 / Verb group
      SIMILAR_TO,    // &  / Adj                  / Similar to
      PA_VERB,       // <  / Adj                  / Participle of verb
      PERTAINYM      // \  / Adv, Adj             / Derived from adjctive, Pertains to noun
   };
   
   extern const char *word_net_indexs[4];
   extern const char **word_net_index_files;
   extern const char **word_net_data_files;
   
   struct index_record_t
   {
      const char *lemma;
      char pos;
      int tagsense_cnt;
      
      int synset_cnt;
      index_record_t **synset_words;

      int symbols_cnt;
      short int *symbols_ptr;
      //index_record_t **synset_subsets;
      rbtree_t *synset_subsets;
      
      list_t *synset_offsets;
      
      bool is_done;
   };
   
   struct failed_record_t
   {
   };
   
   struct search_param_t
   {
      const char *word;
      const char *file_name;
      const char *file_name_data;
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
   void *read_data_file(const char *data_file_name, list_t *synset_offsets, const char *word, int ptr_symbol);
};

#endif
