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

#include <assert.h>
#include <iostream>
#include <string>

#include <include/html_parser.h>
#include <include/url.h>

using namespace std;
using namespace neweraHPC;

namespace search
{   
   HTMLParser::HTMLParser(const char *src_file, const char *dest_file)
   {
      input_file.open(src_file);
      output_file.open(dest_file);
      
      tag_stack = new rbtree(RBTREE_NUM_MANAGED);
      tags      = new rbtree(RBTREE_STR | RBTREE_HASH);
      tags_list = new rbtree(RBTREE_NUM);
   }
   
   HTMLParser::~HTMLParser()
   {
      if(input_file.is_open())
	 input_file.close();
      if(output_file.is_open())
	 output_file.close();

      int tag_count = tags_list->length();
      
      int attr_count;
      int word_count;
      search_tag_t     *search_tag;
      attribute_pair_t *attribute_pair;
      for(int i = 1; i <= tag_count; i++)
      {
	 search_tag = (search_tag_t *)tags_list->search(i);
	 
	 if(search_tag->attributes)
	 {
	    attr_count = search_tag->attributes->length();
	    for(int j = 1; j <= attr_count; j++)
	    {
	       attribute_pair = (attribute_pair_t *)search_tag->attributes->search_inorder_str(j, NULL);
	       if(attribute_pair->rvalue)
		  delete[] (attribute_pair->rvalue);
	       delete[] (attribute_pair->lvalue);
	       delete attribute_pair;
	    }
	    delete (search_tag->attributes);
	 }
	 if(search_tag->words)
	 {
	    word_count = search_tag->words->length();
	    for(int k = 1; k <= word_count; k++)
	    {
	       char *word = (char *)search_tag->words->search(k);
	       delete[] word;
	    }
	    delete (search_tag->words);
	 }
	 
	 delete[] _search_tag_get_name(search_tag);
	 delete search_tag;
      }
      
      delete tags_list;
      delete tags;
      delete tag_stack;
   }

   void HTMLParser::_analyze_attributes(search_tag_t *search_tag, const char *attribute_str)
   {
      search_tag->attributes = new rbtree(RBTREE_STR);
      rbtree *attributes     = (search_tag->attributes);

      const char *lvalue = NULL;
      nhpc_size_t lvalue_len = 0;
      const char *rvalue = NULL;
      nhpc_size_t rvalue_len = 0;

      while(*attribute_str != '\0')
      {
	 rvalue_len = 0;
	 lvalue_len = 0;
	 
	 skip_white_spaces((char **)&attribute_str);
         if(*attribute_str == '\0')
            break;
	 
	 lvalue = attribute_str;
	 int char_value = (int)(*attribute_str);
	 if(char_value < 97 && char_value > 122)
	 {
	    attribute_str++;
	    continue;
	 }
	 
	 while(*attribute_str != '\0' && *attribute_str != ' ' && *attribute_str != '=')
	 {
	    attribute_str++;
	    lvalue_len++;
	 }
	 lvalue = nhpc_substr(lvalue, 1, lvalue_len);

	 attribute_pair_t *attribute_pair = new attribute_pair_t;
	 (*attributes).insert(attribute_pair, lvalue);
	 attribute_pair->lvalue = lvalue;
	 
	 skip_white_spaces((char **)&attribute_str);
	 if(*attribute_str != '=')
	 {
	    continue;
	 }
	 attribute_str++;
	 skip_white_spaces((char **)&attribute_str);
	 char term_char = ' ';
	 if(*attribute_str == '\'')
	 {
	    term_char = '\'';
	    attribute_str++;
	 }
	 else if(*attribute_str == '"')
	 {
	    term_char = '"';
	    attribute_str++;
	 }
	 
	 rvalue = attribute_str;
	 while(*attribute_str != '\0' && *attribute_str != term_char)
	 {
	    attribute_str++;
	    rvalue_len++;
	 }
	 if(*attribute_str == term_char)
	    attribute_str++;
	 
	 rvalue = nhpc_substr(rvalue, 1, rvalue_len);
	 attribute_pair->rvalue = rvalue;
      }
   }
   
   void HTMLParser::_set_tag_params(search_tag_t **search_tag, const char *tag_str)
   {
      (*search_tag) = new search_tag_t;
      memset((*search_tag), 0, sizeof(search_tag_t));

      if(nhpc_strcmp(tag_str, "</*>"))
      {
         _search_tag_set_opt((*search_tag), TAG_CLOSED);
         tag_str += 2;
      }
      else
      {
         _search_tag_set_opt((*search_tag), TAG_OPEN);
         if(nhpc_strcmp(tag_str, "<*/>"))
         {
            _search_tag_set_opt((*search_tag), TAG_CLOSED);
         }
         else
         {
            _search_tag_set_opt((*search_tag), TAG_OPEN);
         }
         tag_str++;
      }

      nhpc_size_t end_pos = nhpc_strfind(tag_str, ' ');
      if(end_pos == 0)
         end_pos = nhpc_strfind(tag_str, '>');
      _search_tag_set_name((*search_tag), nhpc_substr(tag_str, 1, end_pos - 1));
      tag_str += (end_pos - 1);
      skip_white_spaces((char **)&tag_str);

      if(*tag_str == '>' || *tag_str == '/')
         return;

      end_pos = strlen(tag_str);
      if(tag_str[end_pos - 2] == '/')
         end_pos--;

      const char *attribute_str = nhpc_substr(tag_str, 1, end_pos - 1);
      _analyze_attributes((*search_tag), attribute_str);
      delete[] attribute_str;
   }
   
   search_tag_t *HTMLParser::_pop_tag(search_tag_t *match_tag)
   {
      int count = (*tag_stack).length();
      if(count == 0)
      {
         return NULL;
      }
      
      search_tag_t *search_tag = (search_tag_t *)(*tag_stack).search(count);
      if(nhpc_strcmp(_search_tag_get_name(search_tag), _search_tag_get_name(match_tag)) != NHPC_SUCCESS)
      {
         cout << _search_tag_get_name(search_tag) << ":" << _search_tag_get_name(match_tag) << endl; 
         return NULL;
      }

      current_tag = (search_tag_t *)(*tag_stack).search(count - 1);

      (*tag_stack).erase(count);
      return search_tag;
   }

   void HTMLParser::_push_tag(search_tag_t *search_tag)
   {
      (*tag_stack).insert(search_tag);

      current_tag = search_tag;
   }

   void HTMLParser::_add_word(const char *word_str)
   {
      if(current_tag && word_str)
      {
         const char *current_tag_name = _search_tag_get_name(current_tag);
         if(nhpc_strcmp(current_tag_name, "script") == NHPC_SUCCESS)
            return;
         else if(nhpc_strcmp(current_tag_name, "noscript") == NHPC_SUCCESS)
            return;
         else if(nhpc_strcmp(current_tag_name, "style") == NHPC_SUCCESS)
            return;

         char *tmp_str = (char *)word_str;
         while(*tmp_str != '\0')
         {
	    if(*tmp_str != ' ' && *tmp_str != '\'')
	    {
	       if(*tmp_str == '&')
	       {
		  if(nhpc_strcmp(tmp_str, "&amp;*") == NHPC_SUCCESS)
		  {
		     while(*tmp_str != ';')
		     {
			*tmp_str = ' ';
			tmp_str++;
		     }
		     *tmp_str = ' ';
		  }
	       }
	       else
	       {
		  int char_value = (int)(*tmp_str);
		  if(char_value < 97 || char_value > 122)
		     *tmp_str = ' ';
	       }
	    }
            tmp_str++;
         }

         string_t *parts = nhpc_substr(word_str, ' ');
	 rbtree **words = &(current_tag->words);
	 if(!(*words))
	    (*words) = new rbtree(RBTREE_NUM);
         for(int i = 0; i < parts->count; i++)
	 {
	    char *word;
	    nhpc_strcpy(&word, parts->strings[i]);
	    (*words)->insert(word);
	 }
         nhpc_string_delete(parts);
      }
   }

   void HTMLParser::_analyze_tag(const char *tag_str)
   {
      search_tag_t *search_tag;
      _set_tag_params(&search_tag, tag_str);
    
      tags_list->insert(search_tag);
      tags->insert(search_tag, _search_tag_get_name(search_tag));
      
      if(nhpc_strcmp(_search_tag_get_name(search_tag), "meta") == NHPC_SUCCESS)
         return;
      else if(nhpc_strcmp(_search_tag_get_name(search_tag), "link") == NHPC_SUCCESS)
         return;
      else if(nhpc_strcmp(_search_tag_get_name(search_tag), "input") == NHPC_SUCCESS)
         return;
      else if(nhpc_strcmp(_search_tag_get_name(search_tag), "img") == NHPC_SUCCESS)
         return;
      else if(nhpc_strcmp(_search_tag_get_name(search_tag), "br") == NHPC_SUCCESS)
         return;
      
      if(_search_tag_is_open(search_tag) && !_search_tag_is_closed(search_tag))
      {
         _push_tag(search_tag);
      }
      else if(!_search_tag_is_open(search_tag) && _search_tag_is_closed(search_tag))
      {
         assert(_pop_tag(search_tag) != NULL);
      }
   }
   
   void HTMLParser::_parse_tags(const char *line)
   {
      int value;
      int end_pos = 1;
      const char *tag_string;
      
      while(*line != '\0')
      {
	 int  value = (int)(*(line + 1));
	 if((*line == '<') && ((value >= 97 && value <= 122) || *(line + 1) == '/'))
	 {
	 find_end:
	    end_pos = nhpc_strfind(line, '>');
	    if(!end_pos)
	    {
	       bool        found_tag_end = false;
	       string      tmp_line;
	       const char *tmp_str;
	       nhpc_strcpy((char **)&tmp_str, line);
	       
	       while(!found_tag_end && getline(input_file, tmp_line))
	       {
		  {
		     const char *tmp_str2;
		     
		     tmp_str2 = nhpc_strconcat(tmp_str, " ", tmp_line.c_str());
		     delete[] tmp_str;
		     tmp_str = tmp_str2;
		  }
		  
		  if(nhpc_strcmp(tmp_str, "*>*") == NHPC_SUCCESS)
		  {
		     found_tag_end = true;
		  }
	       }
	       line = tmp_str;	       
	       goto find_end;
	    }

	    tag_string = nhpc_substr(line, 1, end_pos);
	    
	    int count = (*tag_stack).length();
	    search_tag_t *current_tag = (search_tag_t *)(*tag_stack).search(count - 1);
	    if(current_tag)
	    {
	       const char *current_tag_name = _search_tag_get_name(current_tag);
	       if(nhpc_strcmp(current_tag_name, "script") != NHPC_SUCCESS)
		  _analyze_tag(tag_string);
	    }
	    else 
	       _analyze_tag(tag_string);
	    delete[] tag_string;
	    
	    line += end_pos;
	 }
	 else if(((int)(*line) >= 97 && (int)(*line) <= 122))
	 {
            const char *temp = line;
            nhpc_size_t pos  = 1;
            while(*line != '\0' && *line != '<')
            {
               line++;
               pos++;
            }
            temp = nhpc_substr(temp, 1, pos - 1);
            _add_word(temp);
            delete[] temp;
	 }
         else
            line++;
      }
      
      return;
   }
   
   void HTMLParser::parse()
   {
      string line;
      const char *line_str;
      
      while(getline(input_file, line))
      {
	 nhpc_size_t end_pos = 1;
	 nhpc_size_t tmp_pos;
	 line_str = nhpc_strtolower((char *)line.c_str());
	 
	 if(nhpc_strcmp(line_str, "*<!--*") == NHPC_SUCCESS)
	 {
	    const char *tmp_str = line_str;
	    
	    while(*line_str != '\0')
	    {
	       if(*line_str == '<' && *(line_str + 1) == '!' && *(line_str + 2) == '-' && *(line_str + 3) == '-')
	       {
		  if(end_pos > 1)
		  {
		     tmp_str = nhpc_substr(tmp_str, 1, end_pos - 1);
		     _parse_tags(tmp_str);
		     delete[] tmp_str;
		  }
		  break;
	       }
	       
	       end_pos++;
	       line_str++;
	    }
	    
	    bool tag_off = false;
	    goto find_comment_end;
	    while(!tag_off)
	    {
	       if(!getline(input_file, line))
		  break;
	       line_str = nhpc_strtolower((char *)line.c_str());
	       
	    find_comment_end:
	       if(nhpc_strcmp(line_str, "*-->*") == NHPC_SUCCESS)
	       {
		  tag_off = true;
	       }
	       else 
		  continue;

	       end_pos = 1;
	       tmp_pos = strlen(line_str);
	       while(*line_str != '\0')
	       {
		  if(*line_str == '-' && *(line_str + 1) == '-' && *(line_str + 2) == '>')
		  {
		     end_pos  += 2;
		     line_str += 3;
		     break;
		  }
		  line_str++;
		  end_pos++;
	       }
	       if((tmp_pos - end_pos) > 0)
	       {
		  tmp_str = nhpc_substr(line_str, 1, (tmp_pos - end_pos));
		  _parse_tags(tmp_str);
		  delete[] tmp_str;
	       }
	    }
	    continue;
	 }
	 _parse_tags(line_str);
      }
   }
   
   void HTMLParser::write_output()
   {
      int tag_count = tags_list->length();
      int attribute_count;
      int word_count;
      char             *word;
      search_tag_t     *search_tag;
      attribute_pair_t *attribute_pair;
      
      for(int i = 1; i <= tag_count; i++)
      {
	 search_tag = (search_tag_t *)(*tags_list).search(i);

	 if(search_tag->words || search_tag->attributes)
	 {
	    output_file << "tag:" << _search_tag_get_name(search_tag) << endl;
	    if(search_tag->attributes)
	    {
	       attribute_count = search_tag->attributes->length();
	       output_file << "attributes:" << attribute_count << endl;
	       for(int k = 1; k <= attribute_count; k++)
	       {
		  attribute_pair = (attribute_pair_t *)(search_tag->attributes->search_inorder_str(k, NULL));
		  output_file << "\"" << attribute_pair->lvalue << "\"";
		  if(attribute_pair->rvalue)
		  {
		     output_file << ":" << "\"" << attribute_pair->rvalue << "\"";
		  }
		  output_file << endl;
	       }
	    }
	    if(search_tag->words)
	    {
	       word_count = search_tag->words->length();
	       output_file << "words:";
	       for(int k = 1; k <= word_count; k++)
	       {
		  word = (char *)search_tag->words->search(k);
		  output_file << " " << word;
	       }
	       output_file << endl;	       
	    }
	    output_file << endl;
	 }
      }
   }
};
