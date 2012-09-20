#include <iostream>
#include <fstream>

#include <include/jarvis_data.h>

using namespace std;

namespace jarvis
{
   jarvis_data_t jarvis_data;
   
   jarvis_data_t::jarvis_data_t()
   {
      jarvis_dictionary = new rbtree_t(NHPC_RBTREE_STR);
      jarvis_dictionary_pos = new rbtree_t(NHPC_RBTREE_NUM);
      failed_searches = new rbtree_t(NHPC_RBTREE_STR);
      morphological_database = new rbtree_t(NHPC_RBTREE_NUM_HASH);
      
      rbtree_t *tmp_rbtrees[POS_COUNT];
      for(int i = 0; i < POS_COUNT; i++)
      {
	 tmp_rbtrees[i] = new rbtree_t(NHPC_RBTREE_STR);
      }
      jarvis_dictionary_pos->insert(tmp_rbtrees[0], ADJ);
      jarvis_dictionary_pos->insert(tmp_rbtrees[1], ADJ_SAT);
      jarvis_dictionary_pos->insert(tmp_rbtrees[2], ADV);
      jarvis_dictionary_pos->insert(tmp_rbtrees[3], NOUN);
      jarvis_dictionary_pos->insert(tmp_rbtrees[4], VERB);
      
      mutex = new nhpc_mutex_t;
   }
   
   jarvis_data_t::~jarvis_data_t()
   {
      
   }
   
   void jarvis_data_t::init_morphological_database()
   {
      exception_files = new char* [4];
      exception_files[0] = nhpc_strconcat(word_net_dir, "/", "adv.exc");
      exception_files[1] = nhpc_strconcat(word_net_dir, "/", "adj.exc");
      exception_files[2] = nhpc_strconcat(word_net_dir, "/", "noun.exc");
      exception_files[3] = nhpc_strconcat(word_net_dir, "/", "verb.exc");
      
      morphological_pair_t **morphological_pairs = new morphological_pair_t* [20];
      for(int i = 0; i < 20; i++)
      {
	 morphological_pairs[i] = new morphological_pair_t;
	 memset((morphological_pairs[i]), 0, sizeof(morphological_pair_t));
      }
      
      nhpc_strcpy((char **)&(morphological_pairs[0]->suffix), "*s");
      morphological_database->insert(morphological_pairs[0], NOUN);
      
      nhpc_strcpy((char **)&(morphological_pairs[1]->suffix), "*ses");
      nhpc_strcpy((char **)&(morphological_pairs[1]->ending), "s");
      morphological_database->insert(morphological_pairs[1], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[2]->suffix), "*xes");
      nhpc_strcpy((char **)&(morphological_pairs[2]->ending), "x");
      morphological_database->insert(morphological_pairs[2], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[3]->suffix), "*zes");
      nhpc_strcpy((char **)&(morphological_pairs[3]->ending), "z");
      morphological_database->insert(morphological_pairs[3], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[4]->suffix), "*ches");
      nhpc_strcpy((char **)&(morphological_pairs[4]->ending), "ch");
      morphological_database->insert(morphological_pairs[4], NOUN);           
      
      nhpc_strcpy((char **)&(morphological_pairs[5]->suffix), "*shes");
      nhpc_strcpy((char **)&(morphological_pairs[5]->ending), "sh");
      morphological_database->insert(morphological_pairs[5], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[6]->suffix), "*men");
      nhpc_strcpy((char **)&(morphological_pairs[6]->ending), "man");
      morphological_database->insert(morphological_pairs[6], NOUN);     
      
      nhpc_strcpy((char **)&(morphological_pairs[7]->suffix), "*ies");
      nhpc_strcpy((char **)&(morphological_pairs[7]->ending), "y");
      morphological_database->insert(morphological_pairs[7], NOUN);           
      
      nhpc_strcpy((char **)&(morphological_pairs[8]->suffix), "*s");
      morphological_database->insert(morphological_pairs[8], VERB);           
      
      nhpc_strcpy((char **)&(morphological_pairs[9]->suffix), "*ies");
      nhpc_strcpy((char **)&(morphological_pairs[9]->ending), "y");
      morphological_database->insert(morphological_pairs[9], VERB);           
      
      nhpc_strcpy((char **)&(morphological_pairs[10]->suffix), "*es");
      nhpc_strcpy((char **)&(morphological_pairs[10]->ending), "e");
      morphological_database->insert(morphological_pairs[10], VERB);                 
      
      nhpc_strcpy((char **)&(morphological_pairs[11]->suffix), "es");
      morphological_database->insert(morphological_pairs[11], VERB);                 
      
      nhpc_strcpy((char **)&(morphological_pairs[12]->suffix), "*ed");
      nhpc_strcpy((char **)&(morphological_pairs[12]->ending), "e");
      morphological_database->insert(morphological_pairs[12], VERB);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[13]->suffix), "*ed");
      morphological_database->insert(morphological_pairs[13], VERB);                 
      
      nhpc_strcpy((char **)&(morphological_pairs[14]->suffix), "*ing");
      nhpc_strcpy((char **)&(morphological_pairs[14]->ending), "e");
      morphological_database->insert(morphological_pairs[14], VERB);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[15]->suffix), "*ing");
      morphological_database->insert(morphological_pairs[15], VERB);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[16]->suffix), "*er");
      morphological_database->insert(morphological_pairs[16], ADJ);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[17]->suffix), "*est");
      morphological_database->insert(morphological_pairs[17], ADJ);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[18]->suffix), "*er");
      nhpc_strcpy((char **)&(morphological_pairs[18]->ending), "e");
      morphological_database->insert(morphological_pairs[18], ADJ);                       
      
      nhpc_strcpy((char **)&(morphological_pairs[19]->suffix), "*est");
      nhpc_strcpy((char **)&(morphological_pairs[19]->ending), "e");
      morphological_database->insert(morphological_pairs[19], ADJ);                             
   }
   
   nhpc_status_t jarvis_data_t::add_word(index_record_t *index_record)
   {
      thread_mutex_lock(mutex, NHPC_THREAD_LOCK_WRITE);
      
      rbtree_t *word_tree = (rbtree_t *)jarvis_dictionary->search(index_record->lemma);
      if(!word_tree)
      {
	 word_tree = new rbtree_t(NHPC_RBTREE_NUM);
	 jarvis_dictionary->insert(word_tree, index_record->lemma);
      }
      
      word_tree->insert(index_record, index_record->pos);
      
      rbtree_t *pos_tree = (rbtree_t *)jarvis_dictionary_pos->search(index_record->pos);
      if(!pos_tree->search(index_record->lemma))
	 pos_tree->insert(index_record, index_record->lemma);
      
      thread_mutex_unlock(mutex, NHPC_THREAD_LOCK_WRITE);
   }
   
   rbtree_t *jarvis_data_t::lookup_word(const char *word)
   {
      thread_mutex_lock(mutex, NHPC_THREAD_LOCK_READ);
      rbtree_t *word_tree = (rbtree_t *)jarvis_dictionary->search(word);
      thread_mutex_unlock(mutex, NHPC_THREAD_LOCK_READ);
      
      nhpc_status_t nrv;
      
      if(!word_tree)
      {
	 nrv = load_word(word);
	 
	 if(nrv == NHPC_SUCCESS)
	 {
	    thread_mutex_lock(mutex, NHPC_THREAD_LOCK_READ);
	    word_tree = (rbtree_t *)jarvis_dictionary->search(word);
	    thread_mutex_unlock(mutex, NHPC_THREAD_LOCK_READ);	 
	 }
      }
      
      return word_tree;
   }
   
   char *jarvis_data_t::morphological_analyses(char *word, int pos)
   {
      const char *file_name;
      int file_index;
      
      if(pos == ADV)
	 file_index = 0;
      else if(pos == ADJ)
	 file_index = 1;
      else if(pos == NOUN)
	 file_index = 2;
      else if(pos == VERB)
	 file_index = 3;
      
      file_name = exception_files[file_index];
      
      ifstream exception_file(file_name);
      string line;
      const char *line_str;
      int cmpr_status;
      bool found_word = false;
      char *morphic_word = NULL;
      
      while(getline(exception_file, line))
      {
	 line_str = line.c_str();
	 int space = nhpc_strfind(line_str, ' ');
	 char *src_word = nhpc_substr(line_str, 1, space - 1);
	 cmpr_status = strcmp(word, src_word);
	 
	 delete[] src_word;
	 
	 if(cmpr_status == 0)
	 {
	    found_word = true;
	    
	    string_t *string = nhpc_substr(line_str, ' ');
	    nhpc_strcpy(&morphic_word, string->strings[1]);
	    nhpc_string_delete(string);
	 }
	 else if(cmpr_status < 0)
	    break;
      }
      
      if(found_word)
      {
	 lookup_word(morphic_word);
	 delete[] morphic_word;
	 
	 return NULL;
      }
      
      morphological_pair_t *morphological_pair;
      for(int i = 1; (morphological_pair = (morphological_pair_t *)morphological_database->search(pos, i)); i++)
      {
	 if(nhpc_strcmp(word, morphological_pair->suffix))
	 {
	    char *new_word;
	    char *root_word = nhpc_substr(word, 1, strlen(word) - strlen(morphological_pair->suffix) + 1);
	    if(morphological_pair->ending)
	    {
	       new_word = nhpc_strconcat(root_word, morphological_pair->ending);
	       delete[] root_word;
	    }
	    else 
	       new_word = root_word;
	    

	    search_param_t *search_param = new search_param_t;
	    memset(search_param, 0, sizeof(search_param_t));
	    search_param->file_name = word_net_index_files[file_index];
	    search_param->word = new_word;
	    read_index_file(search_param);
	    if(search_param->index_record)
	       add_word(search_param->index_record);
	    
	    delete search_param;	    
	    delete[] new_word;
	 }
      }
   }
   
   index_record_t *jarvis_data_t::lookup_word(const char *word, int pos)
   {
      thread_mutex_lock(mutex, NHPC_THREAD_LOCK_READ);
      
      index_record_t *index_record = NULL;
      rbtree_t *pos_tree = (rbtree_t *)jarvis_dictionary_pos->search(pos);
      if(pos_tree)
      {
	 index_record = (index_record_t *)pos_tree->search(word);
      }
      
      thread_mutex_unlock(mutex, NHPC_THREAD_LOCK_READ);
      
      return index_record;
   }
   
   nhpc_status_t jarvis_data_t::load_word(const char *_word)
   {
      thread_mutex_lock(mutex, NHPC_THREAD_LOCK_READ);
      void *test_failed_word = failed_searches->search(_word);
      if(test_failed_word)
	 return NHPC_FAIL;
      thread_mutex_unlock(mutex, NHPC_THREAD_LOCK_READ);
      
      int threads[INDEX_FILES_COUNT];
      search_param_t **search_params = new search_param_t* [INDEX_FILES_COUNT];
      search_param_t *search_param;
      int thread_state = NHPC_THREAD_DEFAULT;
      for(int i = 0; i < INDEX_FILES_COUNT; i++)
      {
	 search_param = new search_param_t;
	 memset(search_param, 0, sizeof(search_param_t));
	 search_param->file_name = word_net_index_files[i];
	 search_param->word = _word;
	 search_params[i] = search_param;
	 
	 if(i == (INDEX_FILES_COUNT - 1))
	 {
	    thread_state = NHPC_THREAD_JOIN;
	 }
	 
	 thread_manager->init_thread(&(threads[i]), NULL);
	 thread_manager->create_thread(&(threads[i]), NULL, (void* (*)(void*))read_index_file, search_param, NHPC_THREAD_DEFAULT);
      }
      
      bool delete_structures = true;
      bool found_word = false;
      
      while(1)
      {
	 for(int i = 0; i < INDEX_FILES_COUNT; i++)
	 {
	    if(!(search_params[i]->is_complete))
	       delete_structures = false;
	 }
	 
	 if(delete_structures)
	 {
	    for(int i = 0; i < INDEX_FILES_COUNT; i++)
	    {
	       if(search_params[i]->index_record)
	       {
		  jarvis_data.add_word(search_params[i]->index_record);
		  found_word = true;
	       }
	       else if(i != 4)
	       {
		  int pos;
		  if(i == 0)
		     pos = ADV;
		  else if(i == 1)
		     pos = ADJ;
		  else if(i == 2)
		     pos = NOUN;
		  else if(i == 3)
		     pos = VERB;
		  
		  char *word_morphic = morphological_analyses((char *)_word, pos);
		  if(word_morphic)
		     found_word = true;
	       }
	       
	       delete (search_params[i]);
	    }	       
	    delete[] search_params;
	    
	    break;
	 }
	 delete_structures = true;
      } 
      
      if(!found_word)
      {
	 failed_record_t *failed_record = new failed_record_t;
	 failed_searches->insert(failed_record, _word);
	 return NHPC_FAIL;
      }
      
      return NHPC_SUCCESS;
   }
};