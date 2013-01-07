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

#ifndef _PARSE_H_
#define _PARSE_H_

#include "words.h"

namespace jarvis
{
#define jv_search_status(p)        ((p)->status)
#define jv_search_is_complete(p)   (jv_search_status(p))
#define jv_set_search_complete(p)  (jv_search_status(p) |= 1)
   
#define jv_get_search_param_pos(s)           ((s)->pos)
#define jv_set_search_param_pos(s, pos)      (jv_set_pos(jv_get_search_param_pos(s), pos))
#define jv_search_param_is_ADV(s)            (jv_pos_is_ADV(jv_get_search_param_pos(s))
#define jv_search_param_is_ADJ(s)	     (jv_pos_is_ADJ(jv_get_search_param_pos(s))
#define jv_search_param_is_NOUN(s)	     (jv_pos_is_NOUN(jv_get_search_param_pos(s))
#define jv_search_param_is_VERB(s)           (jv_pos_is_VERB(jv_get_search_param_pos(s))
#define jv_search_param_is_ADJ_SAT(s)        (jv_pos_is_ADJ_SAT(jv_get_search_param_pos(s))
#define jv_set_search_param_source(s, f)     ((s)->source_file = f)
#define jv_set_search_param_word(s, w)       ((s)->word = w)
#define jv_set_search_param_thread_id(s, id) ((s)->thread_id = id)
#define jv_get_search_param_sources(s)       ((s)->source_file)
#define jv_get_search_param_word(s)          ((s)->word)
#define jv_get_search_param_thread_id(s)     ((s)->thread_id)
   
   struct search_param_t
   {
      const char *word;
      const char *source_file;
      void       *result;
      bool        status;
      jv_pos      pos;
      int         thread_id;
   };
};

#endif