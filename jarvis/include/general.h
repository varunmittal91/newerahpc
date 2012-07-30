#ifndef _JARVIS_GENERAL_H_
#define _JARVIS_GENERAL_H_

#include <neweraHPC/rbtree.h>

using namespace neweraHPC;

namespace jarvis
{
   extern rbtree_t *word_list;
   extern char *word_dir;

   void load_word_library(char *_word_dir);
};

#endif
