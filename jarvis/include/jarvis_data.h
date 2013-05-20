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

#ifndef _JARVIS_DATA_H_
#define _JARVIS_DATA_H_

#include "words.h"

#include <neweraHPC/rbtree.h>
#include <neweraHPC/thread.h>
#include <neweraHPC/error.h>

using namespace neweraHPC;

namespace jarvis
{
#define WORDNET_ROOT_DIR "/jarvis/data/dict"
   
   extern const char *wordnet_index_files[INDEXED_POS_COUNT];
   extern const char *wordnet_data_files[INDEXED_POS_COUNT];
   extern const char *wordnet_exc_files[INDEXED_POS_COUNT];
#define jv_get_wordnet_index_file(i)  ((const char *)wordnet_index_files[i])
#define jv_get_wordnet_data_file(i)   ((const char *)wordnet_data_files[i])
#define jv_get_wordnet_exc_file(i)    ((const char *)wordnet_exc_files[i])
   
   void init_jarvis_data();
   
   class jarvis_data_t
   {
   private:
   public:
   };
};

#endif