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

#include <iostream>
#include <fstream>
#include <string>
#include <neweraHPC/neweraHPC.h>

#include <include/jarvis.h>

using namespace std;
using namespace jarvis;

nhpc_status_t func_trigger_jarvis(nhpc_socket_t *sock, web_ui_elements_t *web_ui_elements)
{
   cout << "Dock application trigger initiated" << endl;
   
   web_ui_elements->elements->add_element(JSON_STRING, "appname", "Dock");
   web_ui_elements->elements->close_element();
}

int main(int argc, char **argv)
{
   jarvis_init(argc, argv); 
      
   //neweraHPC_init(argc, argv);
   
   nhpc_status_t nrv;
   
   word_record_t *word_record;   
   
   char *word = (char *)cmdline_arguments.search("w");
   if(word != NULL)
   {
      word_lookup(word, &word_record);
   }
   
   char *file_input = (char *)cmdline_arguments.search("i");
   if(file_input)
   {
      ifstream file(file_input);
      
      if(file.is_open())
      {
	 string lines;
	 string line;
	 
	 while(getline(file, line))
	 {
	    lines += line;
	 }
	 
	 rbtree_t *sentences = parse_sentence(lines.c_str());
	 
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
	    //establish_relation_type(sentence_type->sentence);
	 }
      }
   }
   
   return 0;
}
