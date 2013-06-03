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

#include <iostream>

#include <neweraHPC/rbtree.h>
#include <neweraHPC/thread.h>
#include <neweraHPC/list.h>
#include <neweraHPC/strings.h>

#include "words_data.h"
#include "parse_index.h"

using namespace std;
using namespace neweraHPC;

namespace jarvis
{      
#define ANTONYM      1   // !  / Adv, Adj, Verb, Noun / Antonym 
#define HYPERNYM     2   // @  / Verb, Noun           / Hypernym
#define I_HYPERNYM   3   // @! / Noun                 / Instance Hypernym
#define HYPONYM      4   // ~  / Verb, Noun           / Hyponym
#define I_HYPONYM    5   // ~i / Noun                 / Instance Hyponym
#define M_HOLONYM    6   // #m / Noun                 / Member holonym
#define S_HOLONYM    7   // #s / Noun                 / Sybstane holonym
#define P_HOLONYM    8   // #p / Noun                 / Part holonym
#define M_MERONYM    9   // %m / Noun                 / Member meronym
#define S_MERONYM    10  // %s / Noun                 / Substance meronym
#define P_MERONYM    11  // %p / Noun                 / Part meronym
#define ATTRIBUTE    12  // =  / Adj, Noun            / Attribute
#define DE_FORM      13  // +  / Verb, Noun           / Derivationally related form
#define D_TOPIC      14  // ;c / Adv, Adj, Verb, Noun / Domain of synset - TOPIC
#define M_TOPIC      15  // -c / Noun                 / Member of this domain - TOPIC
#define D_REGION     16  // ;r / Adv, Adj, Verb, Noun / Domain of synset - REGION 
#define M_REGION     17  // -r / Noun                 / Member of this domain - REGION 
#define D_USAGE      18  // ;u / Adv, Adj, Verb, Noun / Domain of synset - USAGE
#define M_USAGE      19  // -u / Noun                 / Member of this domain - USAGE
#define ENTAILMENT   20  // *  / Verb                 / Entailment
#define CAUSE        21  // >  / Verb                 / Cause
#define A_SEE        22  // ^  / Adj, Verb            / Also see
#define V_GROUP      23  // $  / Verb                 / Verb group
#define SIMILAR_TO   24  // &  / Adj                  / Similar to
#define PA_VERB      25  // <  / Adj                  / Participle of verb
#define PERTAINYM    26  // \  / Adv, Adj             / Derived from adjctive, Pertains to noun
   extern rbtree *relation_types;
   static void _jv_add_relation_type(int v, const char *s)
   {
      int *tmp = new int;
      *tmp = v;
      (*relation_types).insert(tmp, s);
   }
   static int _jv_get_relation_type(const char *s)
   {
      int *tmp = (int *)(*relation_types).search(s);
      if(tmp)
	 return *tmp;
      else 
	 return 0;
   }
   
   struct synset_t
   {
      rbtree     *words;
      rbtree     *relations;
   };
   static void jv_init_synset(synset_t **synset)
   {
      (*synset)            = new synset_t;
      memset((*synset), 0, sizeof(synset_t));
      (*synset)->words     = new rbtree;
      (*synset)->relations = new rbtree(RBTREE_NUM | RBTREE_HASH);
   }
#define jv_set_synset_word(s, w) ((*(*s).words).insert(w))
   static int jv_get_offset_length_synset(synset_t *synset, int relation)
   {
      int length = synset->relations->length(relation);
      return length;
   }
   static int jv_get_offset_synset(synset_t *synset, int relation, int i)
   {
      int *offset = (int *)(synset->relations->search(relation, i));
      if(!offset)
	 return 0;
      else 
	 return (*offset);
   }
#define jv_get_word_length_synset(s) (s->words->length())
#define jv_get_word_synset(s, i)     ((const char *)s->words->search(i))
   
   extern rbtree **word_sets;
   void init_jarvis_words_db();
#define jv_insert_word_set_db(p, e, i) ((word_sets[p])->insert(e, i))
#define jv_get_word_set_db(p, i)       ((synset_t *)(word_sets[p])->search(i))
   
   extern rbtree  *word_list_all;
   extern rbtree **word_list_pos;
#define _jv_get_word_list_pos(w)  ((jv_pos *)(*word_list_all).search(w))
   void init_word_list();

   struct word_def_t
   {
      const char  *word;
      jv_pos       pos;
      rbtree      *pointers;
      rbtree      *offsets;
   };   
   jv_pos     *jv_get_word(const char *word);
   word_def_t *jv_get_word(const char *word, jv_pos pos);
   void        jv_add_word(index_record_t *index_record);
#define _jv_word_def_set_pos(w, p)      (w->pos |= p)
#define _jv_word_def_set_word(w, _word) (nhpc_strcpy((char **)&(w->word), _word))
#define _jv_word_def_set_pointers(w, p) (w->pointers = p)
#define _jv_word_def_set_offsets(w, o)  (w->offsets = o)
};

#endif
