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

int main(int argc, char **argv)
{
   jarvis_init(argc, argv); 

   cout << "JARVIS loaded all the data" << endl;
   
   word_record_t *word_record;
   word_lookup("apple", &word_record);
   //word_lookup("apple", &word_record);
   
   char *word_lookup = (char *)cmdline_arguments.search("w");
   if(word_lookup != NULL)
   {
      char *cmd1 = nhpc_strconcat("wn ", word_lookup, " -hypen");
      char *cmd2 = nhpc_strconcat("wn ", word_lookup, " -hypev");
      system(cmd1);
      system(cmd2);
   }
   
   while(1)
      sleep(1);
   
   return 0;
}
