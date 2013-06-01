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

#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <string>

#include <neweraHPC/json.h>
#include <neweraHPC/neweraHPC.h>

#include <include/parse_index.h>
#include <include/jarvis.h>
#include <include/compare.h>

using namespace std;
using namespace jarvis;

int main(int argc, char **argv)
{
   jarvis_init(argc, argv);
   
   const char *word1 = nhpc_get_cmdline_argument("word1");
   const char *word2 = nhpc_get_cmdline_argument("word2");
   json_t *word_structure1 = NULL, *word_structure2 = NULL;
   
   if(word1)
   {
      cout << "Word:" << word1 << endl;
      nhpc_status_t nrv = jv_get_word_def(word1);
      if(nrv == NHPC_SUCCESS)
      {
	 cout << "Word found and loaded" << endl;
	 jv_extract_word_def(word1);
	 
	 word_structure1 = jv_get_json_structure(word1);
	 (*word_structure1).print();
      }
      else 
      {
	 cout << "word search failed" << endl;
      }
   }
   if(word2)
   {
      cout << "Word:" << word2 << endl;
      nhpc_status_t nrv = jv_get_word_def(word2);
      if(nrv == NHPC_SUCCESS)
      {
	 cout << "Word found and loaded" << endl;
	 jv_extract_word_def(word2);
	 
	 word_structure2 = jv_get_json_structure(word2);
	 (*word_structure2).print();
      }
      else 
      {
	 cout << "word search failed" << endl;
      }
   }
   if(word_structure1 && word_structure2)
   {
      cout << "Words loaded now comparing" << endl;
      
      //json_t *result = jv_compare_json_structure(word_structure1, word_structure2, word1, word2);
      //delete result;
   }
   if(word_structure1)
      delete word_structure1;
   if(word_structure2)
      delete word_structure2;

   //while(1)
      //sleep(1);
}
