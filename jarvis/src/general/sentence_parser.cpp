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

#include <iostream>

#include <include/general.h>
#include <include/words.h>
#include <include/jarvis_data.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   rbtree_t *parse_sentence(const char *in_string)
   {
      bool is_question = false;
      rbtree_t *sentences = new rbtree_t(NHPC_RBTREE_NUM); 
      
      string_t *sentences_tmp = nhpc_substr(in_string, '.');
      for(int i = 0; i < sentences_tmp->count; i++)
      {
         sentence_type_t *sentence_type;

         if(nhpc_strcmp((sentences_tmp->strings[i]), "*?*") != NHPC_SUCCESS)
         {
            sentence_type = new sentence_type_t;
            sentence_type->is_question = false;
            nhpc_strcpy((char **)&(sentence_type->sentence), sentences_tmp->strings[i]);

            sentences->insert(sentence_type);
         }
         else 
         {
            string_t *questions = nhpc_substr((sentences_tmp->strings[i]), '?'); 
            for(int j = 0; j < (questions->count - 1); j++)
            {
               sentence_type = new sentence_type_t;
               sentence_type->is_question = true;
               nhpc_strcpy((char **)&(sentence_type->sentence), questions->strings[j]);

               sentences->insert(sentence_type);
            }

	    sentence_type = new sentence_type_t;
	    sentence_type->is_question = false;
	    nhpc_strcpy((char **)&(sentence_type->sentence), questions->strings[questions->count - 1]);
	    
	    sentences->insert(sentence_type);
         }         
      }      
      
      nhpc_string_delete(sentences_tmp);

      return sentences;
   }
   
   bool match_level(rbtree_t *words1, rbtree_t *words2)
   {
      int word_count1 = words1->ret_count();
      int word_count2 = words2->ret_count();
      
      if(word_count1 != word_count2)
	 return false;
      
      bool match = true;
      
      char *word1, *word2;
      
      for(int i = 1; i <= word_count1; i++)
      {
	 word1 = (char *)(*words1)[i];
	 word2 = (char *)(*words2)[i];
	    
	 if(nhpc_strcmp(word1, word2) != NHPC_SUCCESS)
	 {
	    match = false;
	    break;
	 }
      }
      
      return match;
   }
   
   int match_sense(rbtree_t *sense1, rbtree_t *sense2, int *level1_matched, int *level2_matched)
   {
      int level_matched = 0;
      
      int level1 = sense1->ret_count();
      int level2 = sense1->ret_count();
      
      rbtree_t *words1, *words2;
      bool match;
      
      int loop_count = level1;
      if(level2 < loop_count)
	 loop_count = level2;
      
      cout << loop_count << endl;

      for(int i = 1; i <= loop_count; i++)
      {
	 words1 = (rbtree_t *)(*sense1)[i];
	 words2 = (rbtree_t *)(*sense2)[i];
	    
	 match = match_level(words1, words2);
	    
	 if(match == false)
	 {
	    break;
	 }
	 
	 level_matched++;
      }
      
      *level1_matched = level1 - level_matched;
      if(*level1_matched <= 0)
      {
	 *level1_matched = level1;
      }
      else 
      {
	 *level1_matched = level_matched + 1;
      }	
      
      *level2_matched = level2 - level_matched;
      if(*level2_matched <= 0)
      {
	 *level2_matched = level2;
      }
      else 
      {
	 *level2_matched = level_matched + 1;
      }	
      
      return level_matched;
   }
   
   rbtree_t *establish_sense(rbtree_t *senses_available)
   {
      rbtree_t *final_senses = new rbtree_t;
      
      int senses_count = senses_available->ret_count();
      
      cout << "available words: " << senses_count << endl;
      
      rbtree_t *word_tree1, *word_tree2;
      rbtree_t *word_sense1, *word_sense2;
      int sense_count1, sense_count2;
      
      while(senses_available->ret_count() >= 2)
      {
	 cout << "words available in the tree" << endl;
	 
	 word_tree1 = (rbtree_t *)(*senses_available)[1];
	 word_tree2 = (rbtree_t *)(*senses_available)[2];
	 
	 sense_count1 = word_tree1->ret_count();
	 sense_count2 = word_tree2->ret_count();
	 
	 int sense_id_matched1 = 0;
	 int sense_id_matched2 = 0;
	 int last_match_level = 0;
	 int level_matched;
	 int level_matched1 = 0, level_matched2 = 0;
	 	 
	 for(int i = 1; i <= sense_count1; i++)
	 {
	    for(int j = 1; j <= sense_count2; j++)
	    {
	       word_sense1 = (rbtree_t *)(*word_tree1)[i];
	       word_sense2 = (rbtree_t *)(*word_tree2)[j];
	       
	       level_matched = match_sense(word_sense1, word_sense2, &level_matched1, &level_matched2);
	       
	       if(level_matched > 0)
	       {
		  if(last_match_level == 0 || level_matched > last_match_level)
		  {
		     sense_id_matched1 = i;
		     sense_id_matched2 = j;
		     
		     last_match_level = level_matched;
		  }
	       }
	    }
	 }
	 
	 cout << "current level: " << level_matched << endl;
	 
	 if(level_matched == 0)
	    continue;
	 
	 cout << "level matched: " << last_match_level << endl;
	 cout << "matched sense for 1: " << sense_id_matched1 << " for sense 2: " << sense_id_matched2 << endl;
	 cout << "level1:" << level_matched1 << " level2:" << level_matched2 << endl;

	 bool insert1 = true, insert2 = true;
	 for(int i = 1; i <= final_senses->ret_count(); i++)
	 {
	    word_sense_t *word_sense_type = (word_sense_t *)(*final_senses)[i];
	    
	    void *tmp_address = word_sense_type->word_sense;
	    
	    if(tmp_address == (*word_tree1)[sense_id_matched1])
	    {
	       insert1 = false;
	       
	       if(word_sense_type->matched_level < level_matched1)
	       {
		  word_sense_type->matched_level = level_matched1;
	       }
	    }
	    if(tmp_address == (*word_tree2)[sense_id_matched2])
	    {
	       insert2 = false;
	    }
	 }
	 if(insert1)
	 {
	    word_sense_t *word_sense = new word_sense_t;
	    word_sense->word_sense = (rbtree_t *)(*word_tree1)[sense_id_matched1];
	    word_sense->matched_level = level_matched1;
	    
	    final_senses->insert(word_sense);
	 }
	 if(insert2)
	 {
	    word_sense_t *word_sense = new word_sense_t;
	    word_sense->word_sense = (rbtree_t *)(*word_tree2)[sense_id_matched2];
	    word_sense->matched_level = level_matched2;

	    final_senses->insert(word_sense);
	 }
	 
	 if(level_matched1 <= level_matched2)
	    senses_available->erase(1);
	 else 
	    senses_available->erase(2);
      }
      
      senses_count = final_senses->ret_count();
      
      for(int i = 1; i <= senses_count; i++)
      {
	 word_sense_t *word_sense_type = (word_sense_t *)(*final_senses)[i];
	 rbtree_t *word_sense = word_sense_type->word_sense;
	 int synset_level_count = word_sense_type->matched_level;
	    
	 for(int k = 1; k <= synset_level_count; k++)
	 {
	    for(int x = 1; x < k; x++)
	       cout << "\t";
	       
	    rbtree_t *words = (rbtree_t *)(*word_sense)[k];
	       
	    int word_count = words->ret_count();
	       
	    for(int l = 1; l <= word_count; l++)
	    {
	       char *word = (char *)(*words)[l];
		  
	       cout << word << ",";
	    }
	       
	    cout << endl;
	 }
      }

      return final_senses;
   }
   
   rbtree_t *break_entities(sentence_type_t *sentence)
   {
      cout << sentence->sentence << endl;
      
      rbtree_t *senses_available = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      
      string_t *string = nhpc_substr(sentence->sentence, ' ');
      cout << "Sentence parts: "<<endl;
      for(int i = 0; i < string->count; i++)
      {
	 cout << "\t" << string->strings[i] << endl;
	 
	 rbtree_t *word_tree = jarvis_data.lookup_word(nhpc_strtolower(string->strings[i]));
	 
	 if(word_tree == NULL)
	 {
	    cout << "failed exiting" << endl;
	 }
	 
	 index_record_t *index_record = (index_record_t *)word_tree->search(NOUN);
	 
	 if(index_record)
	 {
	    senses_available->insert(index_record->word_senses);
	 }
      }
      
      nhpc_string_delete(string);
      
      rbtree_t *final_senses = NULL;
      
      final_senses = establish_sense(senses_available);
      delete senses_available;

      return final_senses;
   }
   
   void determine_relation_sentence(rbtree_t *sentences)
   {
      
   }
};
