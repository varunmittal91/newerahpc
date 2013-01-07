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

#include <neweraHPC/strings.h>
#include <string.h>

#include <include/morphological_rules.h>
#include <include/words.h>

using namespace neweraHPC;

namespace jarvis
{
   rbtree *morphological_rules;
   
   void init_morphological_rules()
   {
      morphological_rules = new rbtree(RBTREE_NUM | RBTREE_HASH);
      
      jv_morphological_rule **morphological_pairs = new jv_morphological_rule* [20];
      for(int i = 0; i < 20; i++)
      {
	 morphological_pairs[i] = new jv_morphological_rule;
	 memset((morphological_pairs[i]), 0, sizeof(jv_morphological_rule));
      }
      
      nhpc_strcpy((char **)&(morphological_pairs[0]->suffix), "*s");
      morphological_rules->insert(morphological_pairs[0], NOUN);
      
      nhpc_strcpy((char **)&(morphological_pairs[1]->suffix), "*ses");
      nhpc_strcpy((char **)&(morphological_pairs[1]->replacement), "s");
      morphological_rules->insert(morphological_pairs[1], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[2]->suffix), "*xes");
      nhpc_strcpy((char **)&(morphological_pairs[2]->replacement), "x");
      morphological_rules->insert(morphological_pairs[2], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[3]->suffix), "*zes");
      nhpc_strcpy((char **)&(morphological_pairs[3]->replacement), "z");
      morphological_rules->insert(morphological_pairs[3], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[4]->suffix), "*ches");
      nhpc_strcpy((char **)&(morphological_pairs[4]->replacement), "ch");
      morphological_rules->insert(morphological_pairs[4], NOUN);           
      
      nhpc_strcpy((char **)&(morphological_pairs[5]->suffix), "*shes");
      nhpc_strcpy((char **)&(morphological_pairs[5]->replacement), "sh");
      morphological_rules->insert(morphological_pairs[5], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[6]->suffix), "*men");
      nhpc_strcpy((char **)&(morphological_pairs[6]->replacement), "man");
      morphological_rules->insert(morphological_pairs[6], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[7]->suffix), "*ies");
      nhpc_strcpy((char **)&(morphological_pairs[7]->replacement), "y");
      morphological_rules->insert(morphological_pairs[7], NOUN);           
      
      nhpc_strcpy((char **)&(morphological_pairs[8]->suffix), "*s");
      morphological_rules->insert(morphological_pairs[8], VERB);           
      
      nhpc_strcpy((char **)&(morphological_pairs[9]->suffix), "*ies");
      nhpc_strcpy((char **)&(morphological_pairs[9]->replacement), "y");
      morphological_rules->insert(morphological_pairs[9], VERB);           
      
      nhpc_strcpy((char **)&(morphological_pairs[10]->suffix), "*es");
      nhpc_strcpy((char **)&(morphological_pairs[10]->replacement), "e");
      morphological_rules->insert(morphological_pairs[10], VERB);                 
      
      nhpc_strcpy((char **)&(morphological_pairs[11]->suffix), "es");
      morphological_rules->insert(morphological_pairs[11], VERB);                 
      
      nhpc_strcpy((char **)&(morphological_pairs[12]->suffix), "*ed");
      nhpc_strcpy((char **)&(morphological_pairs[12]->replacement), "e");
      morphological_rules->insert(morphological_pairs[12], VERB);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[13]->suffix), "*ed");
      morphological_rules->insert(morphological_pairs[13], VERB);                 
      
      nhpc_strcpy((char **)&(morphological_pairs[14]->suffix), "*ing");
      nhpc_strcpy((char **)&(morphological_pairs[14]->replacement), "e");
      morphological_rules->insert(morphological_pairs[14], VERB);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[15]->suffix), "*ing");
      morphological_rules->insert(morphological_pairs[15], VERB);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[16]->suffix), "*er");
      morphological_rules->insert(morphological_pairs[16], ADJ);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[17]->suffix), "*est");
      morphological_rules->insert(morphological_pairs[17], ADJ);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[18]->suffix), "*er");
      nhpc_strcpy((char **)&(morphological_pairs[18]->replacement), "e");
      morphological_rules->insert(morphological_pairs[18], ADJ);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[19]->suffix), "*est");
      nhpc_strcpy((char **)&(morphological_pairs[19]->replacement), "e");
      morphological_rules->insert(morphological_pairs[19], ADJ);     
      
      delete[] morphological_pairs;
   }
};
