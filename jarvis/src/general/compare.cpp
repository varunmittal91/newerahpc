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
      
      if(synset_length > 1)
	 (*json).add_element(JSON_ARRAY, "child");      
      for(int i = 1; i <= synset_length; i++)
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
      (*json_structure).add_element(JSON_STRING, "word", word);
      
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
	    }
	 }
      }
   }
   
   bool match_level(json_t *level1, json_t *level2)
   {
      int         word_count1;
      int         word_count2;
      bool        result = true;
      const char *word1, *word2;
      
      (*level1).save_search();
      (*level2).save_search();
      
      if(!json_check_object_is_array((*level1)["words"]) || !json_check_object_is_array((*level2)["words"]))
      {
	 cout << "Error not an words array" << endl;
	 result = false;
	 goto return_result;
      }
      
      word_count1 = (*level1).count();
      word_count2 = (*level2).count();

      if(word_count1 != word_count2)
      {
	 result = false;
	 goto return_result;
      }
      
      for(int i = 1; i <= word_count1; i++)
      {
	 word1 = (*level1)[i];
	 word2 = (*level2)[i];
	 
	 if(strcmp(word1, word2) != 0)
	 {
	    result = false;
	    break;
	 }
      }
      
   return_result:
      (*level1).restore_search();
      (*level2).restore_search();
      
      return result;
   }
   
   bool match_json_sense(json_t *sense1, json_t *sense2, int *level1, int *level2, json_t **result)
   {
      *level1 = 0;
      *level2 = 0;
      
      int         word_count; 
      const char *response;
      
      json_t *primary   = sense1;
      json_t *secondary = sense2;
      bool    complete  = false;
      
      (*primary).save_search();
      (*secondary).save_search();
      
      while(json_check_object_found(response = (*primary)["child"]) && !complete)
      {
	 (*level1)++;
	 
	 if(json_check_object_is_array(response))
	 {
	    int tmp_level1;
	    int tmp_level2;
	    int _tmp_level1 = -1;
	    int _tmp_level2 = -1;
	    
	    int     count = (*primary).count();
	    json_t *tmp_result = NULL;
	    json_t *_tmp_result;

	    for(int i = 1; i <= count; i++)
	    {
	       (*primary).save_search();
	       (*primary)[i];

	       (*primary).save_search();
	       (*secondary).save_search();
	       if(match_json_sense(primary, secondary, &tmp_level1, &tmp_level2, &_tmp_result))
	       {
		  tmp_level1 = tmp_level1 + ((*level1) - 1);
		  
		  if(_tmp_level1 == -1 || (_tmp_level1 > tmp_level1 && _tmp_level2 > tmp_level2))
		  {
		     if(tmp_result)
			delete tmp_result;
		     tmp_result = _tmp_result;
		     
		     _tmp_level1 = tmp_level1;
		     _tmp_level2 = tmp_level2;
		  }
		  complete = true;
	       }
	       (*primary).restore_search();
	       (*secondary).restore_search();

	       (*primary).restore_search();
	    }
	    
	    if(complete)
	    {
	       (*result) = tmp_result;
	       
	       (*level1) = _tmp_level1;
	       (*level2) = _tmp_level2;
	    }
	 }
	 else 
	 {
	    (*secondary).save_search();
	    (*level2) = 0;	    
	    while(json_check_object_found(response = (*secondary)["child"]) && !complete)
	    {
	       (*level2)++;
	       
	       if(json_check_object_is_array(response))
	       {
		  int tmp_level1;
		  int tmp_level2;
		  
		  int _tmp_level1 = -1;
		  int _tmp_level2 = -1;
		  		  
		  int     count = (*secondary).count();
		  json_t *tmp_result = NULL;
		  json_t *_tmp_result;

		  (*primary).restore_search();
		  (*primary).save_search();
		  for(int i = 1; i <= count; i++)
		  {		     
		     (*secondary).save_search();		     
		     (*secondary)[i];
		     
		     (*primary).save_search();
		     (*secondary).save_search();
		     if(match_json_sense(primary, secondary, &tmp_level1, &tmp_level1, &_tmp_result))
		     {
			tmp_level1 = (*level1) + tmp_level1;
			tmp_level2 = (*level2);
			complete = true;
			
			if(_tmp_level1 == -1 || (_tmp_level1 > tmp_level1 && _tmp_level2 > tmp_level2))
			{
			   if(tmp_result)
			      delete tmp_result;
			   tmp_result = _tmp_result;
			   
			   _tmp_level1 = tmp_level1;
			   _tmp_level2 = tmp_level2;
			}
		     }
		     (*primary).restore_search();
		     (*secondary).restore_search();

		     (*secondary).restore_search();
		  }
		  (*primary).restore_search();
		  
		  if(complete)
		  {		     
		     (*result) = tmp_result;
		     
		     (*level1) = _tmp_level1;
		     (*level2) = _tmp_level2;
		  }		  
	       }
	       else 
	       {
		  if(match_level(primary, secondary))
		  {		     
		     (*result) = new json_t;
		     (*(*result)).add_element(JSON_ARRAY, "matched_words");
		     
		     (*primary).save_search();
		     (*primary)["words"];
		     
		     int count = (*primary).count();
		     for(int i = 1; i <= count; i++)
		     {
			(*(*result)).add_element(JSON_STRING, (*primary)[i]);
		     }
		     
		     (*(*result)).close_element();
		     
		     (*primary).restore_search();
		     
		     complete = true;		   
		  }
		  
		  LOG_INFO(*level1);
		  LOG_INFO(*level2);
		  (*primary).save_search();
		  (*secondary).save_search();
		  (*primary)["words"];
		  (*secondary)["words"];
		  int count1 = (*primary).count();
		  int count2 = (*secondary).count();
		  LOG_INFO("\t" << (*primary)[1]);
		  LOG_INFO("\t" << (*secondary)[1]);
		  (*primary).restore_search();
		  (*secondary).restore_search();	
	       }
	    }
	    (*secondary).restore_search();
	 }
      }
      
      (*primary).restore_search();
      (*secondary).restore_search();

      if(complete)
      {
	 (*(*result)).close_element();	 
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

      cout << "Sense count1:" << json1_sense_count << " Sense count2:" << json2_sense_count << endl;
      for(int i = 1; i <= json1_sense_count; i++)
      {
	 for(int j = 1; j <= json2_sense_count; j++)
	 {
	    (*json1).save_search();
	    (*json2).save_search();

	    (*json1)[i];
	    (*json2)[j];	    

	    json_t *result;
	    if(match_json_sense(json1, json2, &level1, &level2, &result))
	    {
	       cout << "Matched sense:" << i << ":" << j << ":" << level1 << ":" << level2 << endl << endl;
	       cout << (*result).get_string() << endl;
	    }
	    count++;
	    
	    (*json1).restore_search();
	    (*json2).restore_search();
	 }
      }
      cout << endl;
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