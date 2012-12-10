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

#include <string>
#include <neweraHPC/neweraHPC.h>

#include <include/jarvis.h>
#include <include/words.h>
#include <include/jarvis_data.h>

using namespace std;
using namespace neweraHPC;
using namespace jarvis;

void jarvis_web_handler(http_data_t *http_data)
{
   cout << "web request encountered" << endl;
   
   const char *test_string = "ape human";
      
   if(http_data->request_get)
   {
      char *string_input = nhpc_substr((http_data->request_get), 3, strlen(http_data->request_get));
      
      nhpc_json_t *json = new nhpc_json_t;
      
      char *sentence_str = NULL;
      
      string_t *pos = nhpc_substr(string_input, '&');
      for(int i = 0; i < pos->count; i++)
      {
	 cout << "pos query: " << pos->strings[i] << endl;
	 
	 string_t *parts = nhpc_substr(pos->strings[i], '=');	 
	 
	 if(nhpc_strcmp(parts->strings[1], "noun") == NHPC_SUCCESS)
	 {
	    if(!sentence_str)
	       nhpc_strcpy(&sentence_str, parts->strings[0]);
	    else 
	    {
	       char *tmp = nhpc_strconcat(sentence_str, " ");
	       sentence_str = nhpc_strconcat(tmp, parts->strings[0]);
	    }

	    json->add_element(JSON_STRING, pos->strings[0], pos->strings[1]);
	 }
	 
	 nhpc_string_delete(parts);
      }
      cout << sentence_str << endl;
      
      rbtree_t *sentences = parse_sentence(sentence_str);
      int sentences_count = sentences->ret_count();
      sentence_type_t *sentence_type;
      for(int i = 1; i <= sentences_count; i++)
      {
	 sentence_type = (sentence_type_t *)sentences->search(i);
	 if(sentence_type->is_question)
	 {
	    cout<<"Question: "<<sentence_type->sentence<<"?"<<endl;
	 }
	 else
	 {
	    cout<<"Declarative: "<<sentence_type->sentence<<"."<<endl;
	 }
	 
	 rbtree_t *entities = break_entities(sentence_type);
	 
	 json->add_element(JSON_ARRAY, "senses");

	 int sense_count = entities->ret_count();
	 for(int i = 1; i <= sense_count; i++)
	 {
	    json->add_element(JSON_OBJECT);
	    
	    word_sense_t *word_sense = (word_sense_t *)(*entities)[i];
	    rbtree_t *word_sense_tree = word_sense->word_sense;
	    
	    int level_count = word_sense->matched_level;
	    
	    for(int j = 1; j <= level_count; j++)
	    {
	       json->add_element(JSON_OBJECT, nhpc_itostr(j));
	       json->add_element(JSON_ARRAY, "words");
	       
	       rbtree_t *word_set = (rbtree_t *)(*word_sense_tree)[j];
	       
	       int word_count = word_set->ret_count();
	       
	       for(int k = 1; k <= word_count; k++)
	       {
		  char *word = (char *)(*word_set)[k];
		  json->add_element(JSON_STRING, "word", word);
	       }
	       
	       json->close_element();
	       json->close_element();
	    }
	    delete word_sense;
	    
	    json->close_element();
	 }
	 
	 json->close_element();
	 json->close_element();
	 
	 delete entities;
	 nhpc_string_delete(pos);
      }		 
      
      const char *output_string = json->get_stream();
      delete json;
      
      nhpc_strcpy(&(http_data->custom_response_data), output_string);
      http_data->custom_response_type = NHPC_FILE;
      
      delete string_input;
   }
}

namespace jarvis
{
   thread_manager_t *thread_manager;
   
   void jarvis_init(int argc, char **argv)
   {
      nhpc_status_t nrv;
      
      neweraHPC_init(argc, argv);

      nrv = init_word_net_database();
      if(nrv != NHPC_SUCCESS)
	 return;
      
      thread_manager = new thread_manager_t;
      jarvis_data.init();
      jarvis_data.init_morphological_database();
      
      cout << "JARVIS loaded all the data" << endl;
      
      http_init();
      http_handler_register("jarvis", (fnc_ptr_nhpc_t)jarvis_web_handler);
      
      nhpc_grid_server_t grid_server;
      nrv = grid_server.grid_server_init();
   }
};
