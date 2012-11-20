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

#ifndef _JARVIS_GENERAL_H_
#define _JARVIS_GENERAL_H_

#include <neweraHPC/rbtree.h>

using namespace neweraHPC;

namespace jarvis
{
   struct sentence_type_t
   {
      bool is_question;
      const char *sentence;
   };

   rbtree_t *parse_sentence(const char *in_string);
   void determine_relation_sentence(rbtree_t *sentences);
   void establish_relation_type(const char *sentence);
   rbtree_t *establish_entities(const char *sentence);
   
   rbtree_t *break_entities(sentence_type_t *sentence);
};

#endif
