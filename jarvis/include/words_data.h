#ifndef _WORDS_DATA_H_
#define _WORDS_DATA_H_

namespace jarvis
{      
#define INDEXED_POS_COUNT 4
#define POS_COUNT         5
   
#define ADV      1   //r
#define ADJ      2   //a
#define NOUN     4   //n
#define VERB     8   //v
#define ADJ_SAT  16  //s
   
   typedef unsigned char jv_pos;
   
   struct _pos_codes
   {
      jv_pos	  int_value;
      char        ascii_value;
      const char *file_suffix;
      const char *ascii_name;
   };
   extern _pos_codes   pos_codes[POS_COUNT];
   
#define jv_get_pos_ascii_code(pos)  ((pos_codes[pos]).ascii_value) 
#define jv_get_pos_int_code(pos)    ((pos_codes[pos]).int_value)
#define jv_get_pos_file_suffix(pos) ((pos_codes[pos]).file_suffix)
#define jv_set_pos(p, pos)          (p |= pos)
#define jv_pos_is_ADV(pos)          ((pos) & 1)
#define jv_pos_is_ADJ(pos)          ((pos >> 1) & 1)
#define jv_pos_is_NOUN(pos)         ((pos >> 2) & 1)
#define jv_pos_is_VERB(pos)         ((pos >> 3) & 1)
#define jv_pos_is_ADJ_SAT(pos)      ((pos >> 4) & 1)
#define jv_get_pos_ascii_name(pos)  ((pos_codes[pos]).ascii_name)
   
#define jv_get_word_pos(w)         ((w)->pos)
#define jv_set_word_pos(w, pos)    (jv_set_pos(jv_get_word_pos(w), pos))
#define jv_word_is_ADV(w)          (jv_pos_is_ADV(jv_get_word_pos(w)))
#define jv_word_is_ADJ(w)          (jv_pos_is_ADJ(jv_get_word_pos(w)))
#define jv_word_is_NOUN(w)         (jv_pos_is_NOUN(jv_get_word_pos(w)))
#define jv_word_is_VERB(w)         (jv_pos_is_VERB(jv_get_word_pos(w)))
#define jv_word_is_ADJ_SAT(w)      (jv_pos_is_ADJ_SAT(jv_get_word_pos(w)))
   
   static int jv_get_pos_int_value(jv_pos pos)
   {
      int i = -1;
      
      do 
      {
	 i++;
	 pos = pos >> 1;
      }while(pos != 0);
      
      return (i);
   }
}

#endif