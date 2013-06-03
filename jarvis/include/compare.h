#include <neweraHPC/json.h>

using namespace neweraHPC;

namespace jarvis
{
   void jv_extract_word_def(const char *word);
   json_t *jv_get_json_structure(const char *word);
   void jv_get_json_structure_recursive(json_t *json, int pos, synset_t *synset, int level = 0);

   json_t *match_json_structures(json_t *json1, json_t *json2);
}