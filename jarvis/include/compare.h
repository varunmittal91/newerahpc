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

#include <neweraHPC/json.h>

using namespace neweraHPC;

namespace jarvis
{
   json_t *match_json_structures(json_t *json1, json_t *json2);

   rbtree * jv_extract_sense_tree(json_t *json);
   void     jv_delete_sense_tree(rbtree *master_tree, int delete_word_tree = 1);
   void     jv_extract_sense_tree_print(rbtree *master_tree);
   void    _jv_extract_sense_tree_merge_clones(rbtree *master_tree, rbtree **master_trees, int branch_count, int clone_count);
   void    _jv_extract_sense_tree_replicate(rbtree *master_tree, int count);
   void    _jv_extract_sense_tree_add_words(rbtree *dest, json_t *json);
}