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

      return sentences;
   }
   
   rbtree_t *break_entities(sentence_type_t *sentence)
   {
      cout << sentence->sentence << endl;
      
      string_t *string = nhpc_substr(sentence->sentence, ' ');
      cout << "Sentence parts: "<<endl;
      for(int i = 0; i < string->count; i++)
      {
	 cout << "\t" << string->strings[i] << endl;
	 
	 word_lookup(nhpc_strtolower(string->strings[i]), NULL);
      }
   }
   
   void determine_relation_sentence(rbtree_t *sentences)
   {
      
   }
};
