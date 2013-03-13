#include <stdlib.h>
#include <iostream>

#include <neweraHPC/json.h>
#include <neweraHPC/strings.h>

#include <include/words.h>

using namespace neweraHPC;
using namespace std;

namespace jarvis
{
   void speak(const char *sentence)
   {
      const char *command = nhpc_strconcat("say ", sentence);
      system(command);
      delete[] command;
   }
   
   void jv_get_json_structure_recursive(json_t *json, int pos, synset_t *synset, int level = 0)
   {
      (*json).add_element(JSON_OBJECT, "child");
      
      (*json).add_element(JSON_ARRAY, "words");
      const char *_word;
      int         word_count = synset->words->length();
      for(int k = 1; k <= word_count; k++)
      {
	 _word = (const char *)synset->words->search(k);
	 (*json).add_element(JSON_STRING, _word);
      }      
      (*json).close_element();
      
      int synset_length   = 0;
      int parent_relation = 0;
      if((synset_length = jv_get_offset_length_synset(synset, HYPERNYM)) > 0)
	 parent_relation = HYPERNYM;
      else if((synset_length = jv_get_offset_length_synset(synset, I_HYPERNYM)) > 0)
	 parent_relation = I_HYPERNYM;
      if(parent_relation == 0)
      {
	 (*json).close_element();
	 return;
      }
      
      synset_length = 1;
      
      if(synset_length > 1)
	 (*json).add_element(JSON_ARRAY, "child");      
      for(int i = synset_length; i > 0; i--)
      {
	 int new_offset = jv_get_offset_synset(synset, parent_relation, i);	 
	 synset_t *new_synset = jv_get_word_set_db(pos, new_offset);
	 if(synset_length > 1)
	 {
	    (*json).add_element(JSON_OBJECT);
	    jv_get_json_structure_recursive(json, pos, new_synset);
	    (*json).close_element();
	 }
	 else 
	    jv_get_json_structure_recursive(json, pos, new_synset);	    
      }    
      if(synset_length > 1)
	 (*json).close_element();
      
      (*json).close_element();
   }
   
   json_t *jv_get_json_structure(const char *word)
   {
      json_t *json_structure = new json_t;
      
      for(int i = 0; i < POS_COUNT; i++)
      {
	 jv_pos pos = jv_get_pos_int_code(i);
	 
	 word_def_t *word_def = jv_get_word(word, pos);
	 if(!word_def)
	    continue;
	 const char *pos_ascii_name = jv_get_pos_ascii_name(i);
	 
	 (*json_structure).add_element(JSON_OBJECT, pos_ascii_name);
	 (*json_structure).add_element(JSON_ARRAY, "senses");

	 rbtree *senses  = word_def->offsets;
	 int sense_count = senses->length();
	 for(int j = 1; j <= senses->length(); j++)
	 {
	    synset_t *synset;
	    int offset = 0;
	    (*senses).search_inorder_num(j, &offset);
	    if(offset != 0)
	    {
	       (*json_structure).add_element(JSON_OBJECT);
	       synset = jv_get_word_set_db(i, offset);
	       jv_get_json_structure_recursive(json_structure, i, synset);
	       (*json_structure).close_element();
	    }
	 }
	 
	 (*json_structure).close_element();
	 (*json_structure).close_element();
      }
      
      (*json_structure).close_element();
      
      return json_structure;
   }
   
   bool jv_match_words(json_t *structure1, json_t *structure2)
   {
      int count1 = (*structure1).count();
      int count2 = (*structure2).count();
      
      if(count1 != count2)
	 return false;
      
      for(int i = 1; i <= count1; i++)
      {
	 const char *word1 = (*structure1)[i];
	 const char *word2 = (*structure2)[i];
	 
	 //cout << word1 << " " << word2 << endl;
	 
	 if(nhpc_strcmp(word1, word2) != NHPC_SUCCESS)
	    return false;
      }

      return true;
   }
   
   int jv_compare_json_structure_word_set(json_t *structure1, json_t *structure2)
   {
      int matched_level = 0;
            
      const char *response1 = NULL;
      const char *response2 = NULL;
      
      while(1)
      {
	 matched_level++;
	 
	 response1 = (*structure1)["child"];
	 if(!json_check_object_found(response1))
	    break;

	 (*structure1).save_search();
	 (*structure1)["words"];
	 
	 (*structure2).save_search();
	 while(1)
	 {
	    response2 = (*structure2)["child"];
	    if(!json_check_object_found(response2))
	       break;
	    
	    (*structure2).save_search();
	    (*structure2)["words"];
	    
	    bool test = jv_match_words(structure1, structure2);
	    if(test)
	    {
	       (*structure1).restore_search();
	       (*structure2).restore_search();
	       (*structure2).restore_search();
	       
	       return matched_level;
	    }
	    
	    (*structure2).restore_search();
	 }
	 (*structure2).restore_search();
	 
	 (*structure1).restore_search();
      }
      
      return -1;
   }
   
   void jv_compare_json_structure_sense(json_t *structure1, json_t *structure2, int *level, int *sense1, int *sense2)
   {
      (*structure1)["senses"];
      (*structure2)["senses"];
      
      int sense_count1 = (*structure1).count();
      int sense_count2 = (*structure2).count();
      
      int matched_1      = 0;
      int matched_2      = 0;
      int matched_level  = 0;
      int _matched_level = 0;
      
      //cout << sense_count1 << ":" << sense_count2 << endl;
      for(int i = 1; i <= sense_count1; i++)
      {
	 (*structure1).save_search();
	 (*structure1)[i];
	 
	 for(int j = 1; j <= sense_count2; j++)
	 {
	    (*structure1).save_search();
	    (*structure2).save_search();
	    (*structure2)[j];
	    _matched_level = jv_compare_json_structure_word_set(structure1, structure2);
	    //cout << "matched level:" << _matched_level << endl;
	    
	    if((matched_level == 0 || _matched_level < matched_level) && _matched_level != -1)
	    {
	       matched_level = _matched_level;
	       matched_1     = i;
	       matched_2     = j;
	    }
	    
	    (*structure2).restore_search();
	    (*structure1).restore_search();
	 }
	 (*structure1).restore_search();
      }
      
      *level  = matched_level;
      *sense1 = matched_1;
      *sense2 = matched_2;
   }
   
   void jv_speak_results(json_t *structure1, json_t *structure2, int pos, int level, int sense1, int sense2)
   {
      (*structure1).reinitialize_search();
      const char *pos_ascii_name = jv_get_pos_ascii_name(pos);
      (*structure1)[pos_ascii_name];
      (*structure1)["senses"];
      (*structure1)[sense1];

      for(int i = 1; i <= level; i++)
      {
	 (*structure1)["child"];
      }
      (*structure1)["words"];
      
      speak("The relationship can be defined by the following words,");
      
      int word_count = (*structure1).count();
      if(word_count > 3)word_count = 3;
      for(int i = 1; i <= word_count; i++)
      {
	 const char *word = (*structure1)[i];
	 speak(word);
      }
      
      jv_pos pos_code = jv_get_pos_int_code(pos);
      if(1)
      {
	 (*structure1).reinitialize_search();
	 (*structure1)[pos_ascii_name];
	 (*structure1)["senses"];
	 (*structure1)[sense1];
	 
	 for(int i = 1; i <= (level + 1); i++)
	 {
	    const char *response = (*structure1)["child"];
	    if(!json_check_object_found(response))
	       return;
	 }
	 (*structure1)["words"];

	 speak("which is a");
	 
	 int word_count = (*structure1).count();
	 if(word_count > 3)word_count = 3;
	 for(int i = 1; i <= word_count; i++)
	 {
	    const char *word = (*structure1)[i];
	    speak(word);
	 }
      }
      
      cout << (*structure1)[1] << endl;
   }
   
   void jv_compare_json_structure(json_t *structure1, json_t *structure2, const char *word1, const char *word2)
   {
      const char *response1, *response2;
      
      for(int i = 0; i < POS_COUNT; i++)
      {
	 (*structure1).reinitialize_search();
	 (*structure2).reinitialize_search();
	 
	 const char *pos_ascii_name = jv_get_pos_ascii_name(i);
	 response1 = (*structure1)[pos_ascii_name];
	 response2 = (*structure2)[pos_ascii_name];
	 if(json_check_object_found(response1) && json_check_object_found(response2))
	 {
	    int level;
	    int sense1;
	    int sense2;
	    
	    cout << pos_ascii_name << endl;
	    jv_compare_json_structure_sense(structure1, structure2, &level, &sense1, &sense2);
	    cout << level << ":" << sense1 << ":" << sense2 << endl;
	    if(level != -1)
	    {
	       speak("Analysis for word");
	       speak(word1);
	       speak("and");
	       speak(word2);
	       speak("complete");
	       speak("Largest hit found in category");
	       speak(pos_ascii_name);
	       jv_speak_results(structure1, structure2, i, level, sense1, sense2);
	    }
	 }
      }
   }
	  
   
   void jv_do_recursive(const char *word, int pos, synset_t *synset, int max_depth)
   {
      if(max_depth == 3)
	 return;
      
      if(max_depth == 0)
	 speak("It can be defined by following words");
      else if(max_depth == 1)
	 speak("Which further can be defined by");
      else if(max_depth == 2)
	 speak("also");
      const char *_word;
      int         word_count = synset->words->length();
      for(int k = 1; k <= word_count; k++)
      {
	 _word = (const char *)synset->words->search(k);
	 speak(_word);
      }      
      
      int synset_length   = 0;
      int parent_relation = 0;
      if((synset_length = jv_get_offset_length_synset(synset, HYPERNYM)) > 0)
	 parent_relation = HYPERNYM;
      else if((synset_length = jv_get_offset_length_synset(synset, I_HYPERNYM)) > 0)
	 parent_relation = I_HYPERNYM;
      if(parent_relation == 0)
	 return;

      for(int i = 1; i <= synset_length; i++)
      {
	 int new_offset = jv_get_offset_synset(synset, parent_relation, i);	 
	 synset_t *new_synset = jv_get_word_set_db(pos, new_offset);
	 jv_do_recursive(word, pos, new_synset, max_depth);
      }
      max_depth++;
   }
   
   void jv_extract_word_def(const char *word)
   {
      for(int i = 0; i < POS_COUNT; i++)
      {
	 jv_pos pos = jv_get_pos_int_code(i);
	 
	 word_def_t *word_def = jv_get_word(word, pos);
	 if(!word_def)
	    continue;
	 
	 rbtree *senses  = word_def->offsets;
	 int sense_count = senses->length();
	 cout << "Sense count:" << sense_count << endl;

	 const char *pos_ascii_name = jv_get_pos_ascii_name(i);
	 
	 for(int j = 1; j <= senses->length(); j++)
	 {
	    synset_t *synset;
	    int offset = 0;
	    (*senses).search_inorder_num(j, &offset);
	    if(offset != 0)
	    {
	       synset = jv_get_word_set_db(i, offset);
	       //jv_do_recursive(word, i, synset, 0);
	    }
	 }
      }
   }
};