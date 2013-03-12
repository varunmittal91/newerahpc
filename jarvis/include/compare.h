#include <neweraHPC/json.h>

using namespace neweraHPC;

namespace jarvis
{
   void jv_extract_word_def(const char *word);
   void speak(const char *);
   json_t *jv_get_json_structure(const char *word);
   void jv_compare_json_structure(json_t *structure1, json_t *structure2, const char *word1, const char *word2);
}