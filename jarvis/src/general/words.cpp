/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com> & Uday Vir Singh <udayvirsingh@gmail.com>
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

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <neweraHPC/neweraHPC.h>

#include <include/general.h>
#include <include/words.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   rbtree_t *word_list;
   rbtree_t *word_types;
   rbtree_t *relations;
   rbtree_t *nodes;
   rbtree_t *functions;
   rbtree_t *verbs;
   rbtree_t *adjectives;
   
   const char *word_dir;
   const char *word_net_dir;  
   const char *word_net_indexs[4] = {"index.adv", "index.adj", "index.noun", "index.verb"};
   const char **word_net_index_files;
   
   nhpc_status_t init_word_net_database()
   {
      nhpc_status_t nrv;
      
      word_dir = (const char *)cmdline_arguments.search("d");
      
      if(word_dir == NULL)
	 word_dir = "data";
      
      nrv = nhpc_fileordirectory(word_dir);
      if(nrv != NHPC_DIRECTORY)
      {
	 LOG_ERROR("Jarvis Data Directory Missing");
	 return NHPC_FAIL;
      }
      
      word_net_dir = nhpc_strconcat(word_dir, "/", "dict");
      if(nrv != NHPC_DIRECTORY)
      {
	 LOG_ERROR("WordNET Database Missing");
	 return NHPC_FAIL;
      }
      
      word_net_index_files = new const char* [4];
      for(int i = 0; i < 4; i++)
      {
	 word_net_index_files[i] = nhpc_strconcat(word_net_dir, "/", word_net_indexs[i]);
	 cout << "loading index file: " << word_net_index_files[i] << endl;
      }
      
      word_list = new rbtree_t(NHPC_RBTREE_STR);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t word_lookup(const char *_word, word_record_t **word_record_ptr)
   {
      word_record_t *word_record = (word_record_t *)word_list->search(_word);
      
      if(!word_record)
      {
	 cout<<"Word record not found"<<endl;
	 word_record = new word_record_t;
	 word_list->insert(word_record, _word);	 
	 
	 int threads[4];
	 int i = 2;
	 //for(int i = 0; i < 1; i++)
	 //{
	    search_param_t *search_param = new search_param_t;
	    search_param->file_name = word_net_index_files[i];
	    search_param->word = _word;
	    
	    thread_manager->init_thread(&(threads[i]), NULL);
	    thread_manager->create_thread(&(threads[i]), NULL, (void* (*)(void*))read_index_file, search_param, NHPC_THREAD_DEFAULT);
	    
	    delete search_param;
	 //}
	 
	 return NHPC_FAIL;
      }
	 	 
      if(!(word_record->has_word))
      {
	 cout<<"Record found but not word"<<endl;
	 return NHPC_FAIL;
      }
      
      return NHPC_SUCCESS;
   }
   
   void *read_index_file(search_param_t *search_param)
   {
      bool found_word = false;
      
      search_param->index_record = NULL;
      
      cout << "searching in file: " << search_param->file_name << endl;
      
      string line;
      ifstream index_file(search_param->file_name);
      int line_number = 1;
      
      while(getline(index_file, line))
      {
	 const char *line_str = line.c_str();
	 
	 int space = nhpc_strfind(line_str, ' ');
	 
	 char *src_word = nhpc_substr(line_str, 1, space - 1);
	 if(src_word != NULL)
	 {
	    int cmpr_status = 0;
	    cout << space << endl;
	    cout << search_param->word << ":" << src_word << ":" <<cmpr_status <<line_number<< endl;
	    cmpr_status = strcmp((search_param->word), src_word);
	    
	    delete[] src_word;
	    
	    if(cmpr_status < 0)
	       break;
	    else if(cmpr_status == 0)
	    {
	       cout << "src_word: " << src_word << endl;
	       
	       found_word = true;
	       break;
	    }
	 }
	 else 
	    cout << "error: " << line_number << search_param->file_name << endl;
	 
	 line_number++;
      }
      
      if(found_word)
	 cout << "found word" << endl;
      
      index_file.close();
   }
};