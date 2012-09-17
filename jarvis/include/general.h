#ifndef _JARVIS_GENERAL_H_
#define _JARVIS_GENERAL_H_

#include <neweraHPC/rbtree.h>

using namespace neweraHPC;

namespace jarvis
{
   struct sentence_type_t
   {
      bool is_question;
      const char *sentence;
   };

   rbtree_t *parse_sentence(const char *in_string);
   void determine_relation_sentence(rbtree_t *sentences);
   void establish_relation_type(const char *sentence);
   rbtree_t *establish_entities(const char *sentence);
};

#endif
