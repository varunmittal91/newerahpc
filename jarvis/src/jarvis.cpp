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

#include <neweraHPC/neweraHPC.h>

#include <include/jarvis.h>
#include <include/words.h>

using namespace neweraHPC;

namespace jarvis
{
   void jarvis_init(int argc, char **argv)
   {
      neweraHPC_init(argc, argv);

      char *jarvis_data = (char *)cmdline_arguments.search("d");
      if(jarvis_data == NULL)
      {
         LOG_ERROR("Jarvis data directory not available");
         exit(0);
      } 

      load_word_library(jarvis_data);
   }
};
