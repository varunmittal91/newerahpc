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

#ifndef _PARSE_INDEX_H_
#define _PARSE_INDEX_H_

#include <neweraHPC/rbtree.h>
#include <neweraHPC/strings.h>

#include "parse.h"

using namespace neweraHPC;

namespace jarvis
{
   struct index_record_t
   {
      const char *word;
      jv_pos      pos;
      rbtree     *pointers;
      rbtree     *data_offsets;
   };
#define jv_set_index_record_word(r, w)           ((r)->word = w)
#define jv_set_index_record_pos(r, p)            ((r)->pos |= p)
#define jv_set_index_record_pointer(r, cp)       ((r)->pointers->insert(cp))
#define jv_set_index_record_offset(r, offset)    ((r)->data_offsets->insert((void *)1, nhpc_strtoi(offset)))
#define jv_get_index_record_word(r)              ((r)->word)
#define jv_get_index_record_pos(r)               ((r)->pos)
#define jv_get_index_record_pointer_count(r)     ((r)->pointers->length())
#define jv_get_index_record_sense_count(r)       ((r)->data_offsets->length())
#define jv_get_index_record_pointer(r, i)        (char *)((r)->pointers->search(i))
#define jv_get_index_record_offset(r, pos, i)    ((r)->data_offsets->search_inorder_num(pos, &i))
#define jv_get_index_record_offsets(r)           (r->data_offsets)
#define jv_get_index_record_pointers(r)          (r->pointers)
   
   void jv_get_index_record(search_param_t *search_param);
   index_record_t *jv_analyze_index_record(search_param_t *search_param, string_t *record_parts);

   nhpc_status_t jv_get_word_def(const char *word);
};

#endif