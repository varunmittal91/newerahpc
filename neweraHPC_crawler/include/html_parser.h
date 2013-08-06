/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v2
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 2 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>

#include <neweraHPC/rbtree.h>
#include <neweraHPC/strings.h>
#include <neweraHPC/constants.h>
#include <neweraHPC/heap.h>

using namespace std;
using namespace neweraHPC;

namespace search
{
   struct search_tag_t
   {
#define TAG_CLOSED 1
#define TAG_OPEN   2
      typedef unsigned char tag_options;
      
      const char   *tag;
      rbtree       *attributes;
      rbtree       *words;
      tag_options   options;
   };
#define _search_tag_set_name(s, n)  (s->tag = n)
#define _search_tag_set_opt(s, o)   (s->options |= o)
#define _search_tag_get_name(s)     (s->tag)
   static bool _search_tag_is_opt(search_tag_t *search_tag, int opt)
   {
      int bit = -1;
      while(opt > 0)
      {
	 opt = opt / 2;
	 bit++;
      
      }
      return (((search_tag->options) >> bit) & 1);
   }
#define _search_tag_is_open(s)   (_search_tag_is_opt(s, TAG_OPEN))  
#define _search_tag_is_closed(s) (_search_tag_is_opt(s, TAG_CLOSED))   
   struct attribute_pair_t
   {
      const char *lvalue;
      const char *rvalue;
   };   
   
   static const char *_search_tag_get_attribute(search_tag_t *search_tag, const char *lvalue)
   {
      if(!(search_tag->attributes))
         return NULL;
      attribute_pair_t *attribute_pair = (attribute_pair_t *)(search_tag->attributes->search(lvalue));
      if(attribute_pair)
	 return attribute_pair->rvalue;
      else 
	 return NULL;
   }
      
   class HTMLParser
   {
   private:
      ifstream input_file;
      ofstream output_file;

      rbtree *tag_stack;
      rbtree *tags;
      rbtree *tags_list;
          
      search_tag_t *current_tag;
      void _add_word(const char *word);

      bool _collect_data();
      void _push_tag(search_tag_t *search_tag);
      search_tag_t *_pop_tag(search_tag_t *match_tag);
      
      void _set_tag_params(search_tag_t **search_tag, const char *tag_str);
      void _analyze_attributes(search_tag_t *search_tag, const char *tag_str);
      void _analyze_tag(const char *line);
      void _parse_tags(const char *line);
   public:
      HTMLParser(const char *src_file, const char *dest_file);
      ~HTMLParser();

      void parse();
      void write_output();
   };
};
