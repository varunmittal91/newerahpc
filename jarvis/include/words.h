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
#include <neweraHPC/strings.h>

using namespace neweraHPC;

namespace jarvis
{      
#define INDEXED_POS_COUNT 4
#define POS_COUNT         5
   
#define ADV      1   //r
#define ADJ      2   //a
#define NOUN     4   //n
#define VERB     8   //v
#define ADJ_SAT  16  //s

   typedef unsigned char jv_pos;
   
   struct _pos_codes
   {
      jv_pos	  int_value;
      char        ascii_value;
      const char *file_suffix;
   };
   extern _pos_codes pos_codes[POS_COUNT];
   
   struct word_def_t
   {
      const char  *word;
      jv_pos       pos;
      rbtree      *pointers;
      rbtree      *offsets;
   };
   
#define jv_get_pos_ascii_code(pos)  ((pos_codes[pos]).ascii_value) 
#define jv_get_pos_int_code(pos)    ((pos_codes[pos]).int_value)
#define jv_get_pos_file_suffix(pos) ((pos_codes[pos]).file_suffix)
#define jv_set_pos(p, pos)          (p |= pos)
#define jv_pos_is_ADV(pos)          ((pos) & 1)
#define jv_pos_is_ADJ(pos)          ((pos >> 1) & 1)
#define jv_pos_is_NOUN(pos)         ((pos >> 2) & 1)
#define jv_pos_is_VERB(pos)         ((pos >> 3) & 1)
#define jv_pos_is_ADJ_SAT(pos)      ((pos >> 4) & 1)
   
#define jv_get_word_pos(w)         ((w)->pos)
#define jv_set_word_pos(w, pos)    (jv_set_pos(jv_get_word_pos(w), pos))
#define jv_word_is_ADV(w)          (jv_pos_is_ADV(jv_get_word_pos(w)))
#define jv_word_is_ADJ(w)          (jv_pos_is_ADJ(jv_get_word_pos(w)))
#define jv_word_is_NOUN(w)         (jv_pos_is_NOUN(jv_get_word_pos(w)))
#define jv_word_is_VERB(w)         (jv_pos_is_VERB(jv_get_word_pos(w)))
#define jv_word_is_ADJ_SAT(w)      (jv_pos_is_ADJ_SAT(jv_get_word_pos(w)))
      
   static int jv_get_pos_int_value(jv_pos pos)
   {
      int i = -1;
      
      do 
      {
	 i++;
	 pos = pos >> 1;
      }while(pos != 0);
      
      return (i);
   }
   
   struct ptr_symbol_set_t
   {
      int   ptr_symbol_num;
      char *ptr_symbol_str;
   };
   
#define ANTONYM      0   // !  / Adv, Adj, Verb, Noun / Antonym 
#define HYPERNYM     1   // @  / Verb, Noun           / Hypernym
#define I_HYPERNYM   2   // @! / Noun                 / Instance Hypernym
#define HYPONYM      3   //    / Verb, Noun           / Hyponym
#define I_HYPONYM    4   // i  / Noun                 / Instance Hyponym
#define M_HOLONYM    5   // #m / Noun                 / Member holonym
#define S_HOLONYM    6   // #s / Noun                 / Sybstane holonym
#define P_HOLONYM    7   // #p / Noun                 / Part holonym
#define M_MERONYM    8   // %m / Noun                 / Member meronym
#define S_MERONYM    9   // %s / Noun                 / Substance meronym
#define P_MERONYM    10  // %p / Noun                 / Part meronym
#define ATTRIBUTE    11  // =  / Adj, Noun            / Attribute
#define DE_FORM      12  // +  / Verb, Noun           / Derivationally related form
#define D_TOPIC      13  // ;c / Adv, Adj, Verb, Noun / Domain of synset - TOPIC
#define M_TOPIC      14  // -c / Noun                 / Member of this domain - TOPIC
#define D_REGION     15  // ;r / Adv, Adj, Verb, Noun / Domain of synset - REGION 
#define M_REGION     16  // -r / Noun                 / Member of this domain - REGION 
#define D_USAGE      17  // ;u / Adv, Adj, Verb, Noun / Domain of synset - USAGE
#define M_USAGE      18  // -u / Noun                 / Member of this domain - USAGE
#define ENTAILMENT   19  // *  / Verb                 / Entailment
#define CAUSE        20  // >  / Verb                 / Cause
#define A_SEE        21  // ^  / Adj, Verb            / Also see
#define V_GROUP      22  // $  / Verb                 / Verb group
#define SIMILAR_TO   23  // &  / Adj                  / Similar to
#define PA_VERB      24  // <  / Adj                  / Participle of verb
#define PERTAINYM    25  // \  / Adv, Adj             / Derived from adjctive, Pertains to noun   
};

#endif
