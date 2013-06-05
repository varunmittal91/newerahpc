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

#include <stdlib.h>
#include <iostream>

#include <neweraHPC/json.h>
#include <neweraHPC/strings.h>
#include <neweraHPC/error.h>

#include <include/words.h>

using namespace neweraHPC;
using namespace std;

namespace jarvis
{         
   void _jv_extract_sense_tree_add_words(rbtree *dest, json_t *json)
   {
      rbtree *word_tree = new rbtree(RBTREE_NUM);
      
      (*json).save_search();
      (*json)["words"];
      int         count = (*json).count();
      const char *word;
      for(int i = 1; i <= count; i++)
      {
	 nhpc_strcpy((char **)&(word), (*json)[i]);
	 (*word_tree).insert((void *)word);
      }
      (*json).restore_search();
      
      (*dest).insert(word_tree);
   }
   
   void _jv_extract_sense_tree_replicate(rbtree *master_tree, int count)
   {
      count--;
      
      rbtree *tree = (rbtree *)(*master_tree)[1];
      rbtree *word_tree;
      int     word_tree_count = (*tree).length();
      
      rbtree **new_trees = new rbtree* [count];
      for(int i = 0; i < count; i++)
      {
	 new_trees[i] = new rbtree(RBTREE_NUM);
      }
      
      for(int i = 1; i <= word_tree_count; i++)
      {
	 word_tree = (rbtree *)(*tree)[i];
	 for(int j = 0; j < count; j++)
	    (*(new_trees[j])).insert(word_tree);
      }
      
      for(int i = 0; i < count; i++)
	 (*master_tree).insert(new_trees[i]);
   }
   
   void _jv_extract_sense_tree_merge_clones(rbtree *master_tree, rbtree **master_trees, int branch_count, int clone_count)
   {
      int pos = 1;
      rbtree *tree;
      rbtree *src_tree;
      rbtree *word_tree;
      int src_tree_count;
      int word_tree_count;
      
      for(int i = 0; i < branch_count; i++)
      {
	 src_tree_count = (*(master_trees[i])).length();
	 
	 for(int j = 1; j <= src_tree_count; j++)
	 {
	    tree     = (rbtree *)(*master_tree)[pos];
	    src_tree = (rbtree *)(*(master_trees[i]))[j];
	    
	    word_tree_count = (*src_tree).length();
	    for(int k = 1; k <= word_tree_count; k++)
	    {
	       word_tree = (rbtree *)(*src_tree)[k];
	       (*tree).insert(word_tree);
	    }
	    
	    pos++;
	 }
      }
   }

   rbtree *jv_extract_sense_tree(json_t *json)
   {
      rbtree *master_tree = new rbtree(RBTREE_NUM);
      rbtree *tree        = new rbtree(RBTREE_NUM);
      (*master_tree).insert(tree);
      
      const char *response;

      (*json).save_search();
      
      while(json_check_object_found(response = (*json)["child"]))
      {
	 rbtree *new_master_tree;
	 
	 if(json_check_object_is_array(response))
	 {
	    int clone_count = 0;
	    
	    int count = (*json).count();
	    rbtree **master_trees = new rbtree* [count];
	    
	    for(int i = 1; i <= count; i++)
	    {
	       (*json).save_search();
	       (*json)[i];	       
	       new_master_tree  = jv_extract_sense_tree(json);
	       (*json).restore_search();
	       	       
	       master_trees[i - 1]  = new_master_tree;
	       clone_count         += (*new_master_tree).length();
	    }	    	    
	    _jv_extract_sense_tree_replicate(master_tree, clone_count);
	    _jv_extract_sense_tree_merge_clones(master_tree, master_trees, count, clone_count);
	    
	    break;
	 }
	 else 
	 {
	    _jv_extract_sense_tree_add_words(tree, json);
	 }
      }
      
      (*json).restore_search();
      
      return master_tree;
   }
   
   void jv_extract_sense_tree_print(rbtree *master_tree)
   {
      int     tree_count = (*master_tree).length();
      int     word_count;
      int     word_tree_count;
      rbtree *tree;
      rbtree *words;
      rbtree *word_tree;

      const char *word;
      
      for(int i = 1; i <= tree_count; i++)
      {
	 tree = (rbtree *)(*master_tree)[i];
	 word_tree_count = (*tree).length();

	 for(int j = 1; j <= word_tree_count; j++)
	 {
	    for(int k = 1; k < j; k++)
	       cout << " ";
	    cout << "-->";
	    
	    word_tree = (rbtree *)(*tree)[j];
	    
	    word_count = (*word_tree).length();
	    for(int k = 1; k <= word_count; k++)
	    {
	       word = (const char *)(*word_tree)[k];
	       cout << word << " ";
	    }
	    cout << endl;
	 }
      }
   }
   
   void jv_extract_sense_tree_print_word_tree(rbtree *tree)
   {
      const char *word;
      rbtree     *word_tree;
      int         word_tree_count = (*tree).length();
      int         word_count;
      
      for(int j = 1; j <= word_tree_count; j++)
      {
	 for(int k = 1; k < j; k++)
	    cout << " ";
	 cout << "-->";
	 
	 word_tree = (rbtree *)(*tree)[j];
	 
	 word_count = (*word_tree).length();
	 for(int k = 1; k <= word_count; k++)
	 {
	    word = (const char *)(*word_tree)[k];
	    cout << word << " ";
	 }
	 cout << endl;
      }      
   }
   
   bool jv_compare_sense_tree_word_tree_level(rbtree *level1, rbtree *level2)
   {
      int level1_count = (*level1).length();
      int level2_count = (*level2).length();
      
      if(level1_count != level2_count)
	 return false;
      
      const char *word1, *word2;
      for(int i = 1; i <= level1_count; i++)
      {
	 word1 = (const char *)(*level1)[i];
	 word2 = (const char *)(*level2)[i];
	 
	 if(strcmp(word1, word2) != 0)
	    return false;
      }
      
      return true;
   }
   
   bool jv_compare_sense_tree_word_tree(rbtree *word_tree1, rbtree *word_tree2, int *level1, int *level2)
   {
      rbtree *word_tree_level1;
      rbtree *word_tree_level2;
      
      int word_tree_level_count1 = (*word_tree1).length();
      int word_tree_level_count2 = (*word_tree2).length();
      
      bool complete_true = false;
      
      *level1 = 0;
      *level2 = 0;
      
      for(int i = 1; i <= word_tree_level_count1 && !complete_true; i++)
      {
	 word_tree_level1 = (rbtree *)(*word_tree1)[i];
	 
	 for(int j = 1; j <= word_tree_level_count2 && !complete_true; j++)
	 {
	    word_tree_level2 = (rbtree *)(*word_tree2)[j];
	    
	    if(jv_compare_sense_tree_word_tree_level(word_tree_level1, word_tree_level2))
	    {
	       complete_true = true;

	       *level1 = i;
	       *level2 = j;
	    }
	 }
      }
      
      return complete_true;
   }
   
   bool jv_compare_sense_tree(rbtree *sense1, rbtree *sense2, int *level_match1, int *level_match2, int *subtree1, int *subtree2)
   {
      int subsense_count1 = (*sense1).length();
      int subsense_count2 = (*sense2).length();
      
      rbtree *tree1, *tree2;
      int tmp_level_match1;
      int tmp_level_match2;
      
      (*level_match1) = -1;
      (*level_match2) = -1;
      
      bool complete = false;
      
      for(int i = 1; i <= subsense_count1; i++)
      {
	 tree1 = (rbtree *)(*sense1)[i];
	 
	 for(int j = 1; j <= subsense_count2; j++)
	 {
	    tree2 = (rbtree *)(*sense2)[j];
	    if(jv_compare_sense_tree_word_tree(tree1, tree2, &tmp_level_match1, &tmp_level_match2))
	    {
	       if((*level_match1) == -1 || ((*level_match1) > tmp_level_match1 && (*level_match2) > tmp_level_match2) || tmp_level_match1 == 1 
		  || tmp_level_match2 == 1)
	       {
		  (*level_match1) = tmp_level_match1;
		  (*level_match2) = tmp_level_match2;
		  
		  (*subtree1) = i;
		  (*subtree2) = j;
	       }
	       
	       /*
	       jv_extract_sense_tree_print_word_tree(tree1);
	       jv_extract_sense_tree_print_word_tree(tree2);
	       
	       cout << "Level matched at " << tmp_level_match1 << endl;
	       cout << "Level matched at " << tmp_level_match2 << endl;
		*/
	       
	       complete = true;
	    }
	 }
      }
      
      return complete;
   }
   
   void match_json_senses(json_t *json1, json_t *json2)
   {
      int json1_sense_count = (*json1).count();
      int json2_sense_count = (*json2).count();
      
      int *match_results = new int [json1_sense_count * json2_sense_count];
      int  count         = 0;
      
      int level1;
      int level2;
      
      rbtree *word_sense_tree1, *word_sense_tree2;
      rbtree *tmp_tree;
      word_sense_tree1 = new rbtree(RBTREE_NUM);
      word_sense_tree2 = new rbtree(RBTREE_NUM);
      
      for(int i = 1; i <= json1_sense_count; i++)
      {
	 (*json1).save_search();
	 (*json1)[i];
	 
	 tmp_tree = jv_extract_sense_tree(json1);
	 (*word_sense_tree1).insert(tmp_tree);
	 
	 (*json1).restore_search();
      }
      
      for(int j = 1; j <= json2_sense_count; j++)
      {
	 (*json2).save_search();
	 (*json2)[j];
	 
	 tmp_tree = jv_extract_sense_tree(json2);
	 (*word_sense_tree2).insert(tmp_tree);
	 
	 (*json2).restore_search();
      }
      
      int _sense1;
      int _sense2;
      int subtree1;
      int subtree2;
      int tmp_level_match1;
      int tmp_level_match2;
      int level_match1 = -1;
      int level_match2 = -1;
      bool found_result = false;
      
      rbtree *sense1, *sense2;
      for(int i = 1; i <= json1_sense_count; i++)
      {
	 sense1 = (rbtree *)(*word_sense_tree1)[i];
	 
	 for(int j = 1; j <= json2_sense_count; j++)
	 {
	    sense2 = (rbtree *)(*word_sense_tree2)[j];
	    
	    if(jv_compare_sense_tree(sense1, sense2, &tmp_level_match1, &tmp_level_match2, &subtree1, &subtree2))
	    {
	       if(level_match1 == -1 || (level_match1 > tmp_level_match1 && level_match2 > tmp_level_match2) || tmp_level_match1 == 1
		  || tmp_level_match2 == 1)
	       {
		  _sense1 = i;
		  _sense2 = j;
	       
		  level_match1 = tmp_level_match1;
		  level_match2 = tmp_level_match2;
	       }
	       
	       found_result = true;
	    }
	 }
      }
      
      if(found_result)
      {
	 cout << "Sense1:" << _sense1 << " Sense2:" << _sense2 << endl;
	 cout << "Subtree1:" << subtree1 << " Subtree2:" << subtree2 << endl;
	 cout << "Level1:" << level_match1 << " Level2:" << level_match2 << endl;
	 
	 sense1 = (rbtree *)(*word_sense_tree1)[_sense1];
	 sense2 = (rbtree *)(*word_sense_tree2)[_sense2];
	 
	 rbtree *tmp1, *tmp2;
	 tmp1 = (rbtree *)(*sense1)[subtree1];
	 tmp2 = (rbtree *)(*sense2)[subtree2];
	 jv_extract_sense_tree_print_word_tree(tmp1);
	 jv_extract_sense_tree_print_word_tree(tmp2);
      }
   }
   
   json_t *match_json_structures(json_t *json1, json_t *json2)
   {
      jv_pos      pos;
      const char *pos_ascii_name;
      const char *response1, *response2;
      
      for(int i = 0; i < POS_COUNT; i++)
      {
         (*json1).reinitialize_search();
         (*json2).reinitialize_search();
         
         pos_ascii_name = jv_get_pos_ascii_name(i);
         response1 = (*json1)[pos_ascii_name];
         response2 = (*json2)[pos_ascii_name];
         
         if(json_check_object_found(response1) && json_check_object_found(response2))
         {
            cout << "matching for sense:" << pos_ascii_name << endl;
            (*json1)["senses"];
            (*json2)["senses"];
            match_json_senses(json1, json2);
         }
      }
   }
};