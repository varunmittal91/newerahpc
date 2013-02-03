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

#ifndef _MORPHOLOGICAL_RULES_H_
#define _MORPHOLOGICAL_RULES_H_

#include <neweraHPC/rbtree.h>

#include "parse_index.h"

namespace jarvis
{
   extern neweraHPC::rbtree *morphological_rules;
   
   struct jv_morphological_rule
   {
      const char *suffix;
      const char *replacement;
   }; 

   typedef unsigned char jv_morphic_status;
#define jv_get_morphic_status_count(s)    (int)(s >> 1)
#define jv_get_morphic_status(s)          (s & 1)
#define jv_set_morphic_status_count(s, c) (s |= (c << 1))
#define jv_set_morphic_status_failed(s)   (s |= 1)
#define jv_morphic_status_is_failed(s)    (jv_get_morphic_status(s))
#define jv_morphic_status_is_on(s)        (!(jv_get_morphic_status(s)))

#define jv_get_morphological_rule(pos, i)         (jv_morphological_rule *)((*morphological_rules).search(pos, i))
#define jv_get_morphological_rule_suffix(r)       ((const char *)r->suffix)
#define jv_get_morphological_rule_replacement(r)  ((const char *)r->replacement)
   
   void init_morphological_rules();
   char *morphological_word_search(search_param_t *search_param, jv_morphic_status *search_status);
};

#endif
