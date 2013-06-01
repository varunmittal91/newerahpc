#include <stdlib.h>
#include <iostream>

#include <neweraHPC/json.h>
#include <neweraHPC/strings.h>

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
};