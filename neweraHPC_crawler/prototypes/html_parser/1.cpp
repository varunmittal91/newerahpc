#include <iostream>
#include <fstream>

#include <neweraHPC/strings.h>
#include <neweraHPC/rbtree.h>

using namespace std;
using namespace neweraHPC;

struct search_tag_t
{
#define TAG_CLOSED 1
#define TAG_OPEN   2
   typedef unsigned char tag_options;

   const char   *tag;
   rbtree      **attributes;
   tag_options   options;
};
#define _search_tag_set_opt(s, o)   (s->options |= o)
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
#define _search_tag_is_open_tag(s)  (_search_tag_is_opt(s, TAG_OPEN))  
#define _search_tag_is_close_tag(s) (_search_tag_is_opt(s, TAG_CLOSED))
struct attribute_pair_t
{
   const char *lvalue;
   const char *rvalue;
};

nhpc_status_t _search_parser_get_tag_attributes(const char *string, rbtree **attributes)
{
   int pos = 0;

   (*attributes) = new rbtree(RBTREE_NUM);
   const char *lvalue = NULL;
   nhpc_size_t lvalue_len = 0;
   const char *rvalue = NULL;
   nhpc_size_t rvalue_len = 0;
   
   pos = nhpc_strfind(string, '>');
   char *attr_str = nhpc_substr(string, 2, pos - 1);   
   char *tmp_attr_str = attr_str;

   while(*tmp_attr_str != '\0')
   {
      attribute_pair_t *attribute_pair = new attribute_pair_t;
      (*attributes)->insert(attribute_pair);
      
      rvalue_len = 0;
      lvalue_len = 0;
      
      skip_white_spaces(&tmp_attr_str);
      
      lvalue = tmp_attr_str;
      while(*tmp_attr_str != '\0' && *tmp_attr_str != ' ' && *tmp_attr_str != '=')
      {
	 tmp_attr_str++;
	 lvalue_len++;
      }
      lvalue = nhpc_substr(lvalue, 1, lvalue_len);
      attribute_pair->lvalue = lvalue;
      
      skip_white_spaces(&tmp_attr_str);
      if(*tmp_attr_str != '=')
      {
	 continue;
      }
      tmp_attr_str++;
      
      rvalue = tmp_attr_str;
      while(*tmp_attr_str != '\0' && *tmp_attr_str != ' ')
      {
	 tmp_attr_str++;
	 rvalue_len++;
      }
      rvalue = nhpc_substr(rvalue, 1, rvalue_len);
      attribute_pair->rvalue = rvalue;
   }
}

nhpc_status_t _search_parser_get_first_tag(const char **_input_string, search_tag_t **search_tag)
{
   nhpc_status_t nrv;
   nhpc_size_t   tag_len = 0;
   nhpc_size_t   attribute_len = 0;
   const char   *tag_str = NULL;
   bool          tag_found = false;
   const char   *attribute_str = NULL;
   rbtree       *attributes = NULL;

   (*search_tag) = new search_tag_t;
   memset((*search_tag), 0, sizeof(search_tag_t));
   
   const char *input_string = *_input_string;

   while(*input_string != '\0')
   {
      if(*input_string == '<')
      {
         input_string++;
         if(*input_string == '/')
         {
            _search_tag_set_opt((*search_tag), TAG_CLOSED);
            input_string++;
         }
         else
            _search_tag_set_opt((*search_tag), TAG_OPEN);

         tag_str = input_string;
         break;
      }

      input_string++;
   }

   while(*input_string != '\0')
   {
      if(*(input_string) == ' ')
      {
         tag_found = true;
         if(!attribute_str)
            attribute_str = input_string;
      }
      if(*input_string == '>')
      {
         if(*(input_string - 1) == '/')
            _search_tag_set_opt((*search_tag), TAG_CLOSED);
         break;
      }

      input_string++;
      if(!tag_found)
         tag_len++;
      else
         attribute_len++;
   }

   char *tag = new char [tag_len + 1];
   memcpy((void *)tag, (tag_str), tag_len);
   tag[tag_len] = '\0';
   (*search_tag)->tag = tag;

   if(attribute_str)
      _search_parser_get_tag_attributes(attribute_str, &attributes);
   
   if(attributes)
   {
      for(int i = 1; i <= (*attributes).length(); i++)
      {
	 attribute_pair_t *attribute = (attribute_pair_t *)(*attributes).search(i);
	 if(attribute->lvalue)
	    cout << attribute->lvalue;
	 if(attribute->rvalue)
	    cout << ":" << attribute->rvalue;
	 cout << endl;
      }
   }
   
   *_input_string = input_string;
}

nhpc_status_t _search_parser_get_tags(const char *input_string)
{
   int value = 0;

   while(*input_string != '\0')
   {
      if(*input_string == '<')
      {
         value = (int)(*(input_string + 1));
         if((value >= 97 && value <= 122) || *(input_string + 1) == '/') 
         {
            search_tag_t *search_tag;
            _search_parser_get_first_tag(&input_string, &search_tag);
            cout << search_tag->tag << " ";
            if(_search_tag_is_open_tag(search_tag))
               cout << ":Open";
            if(_search_tag_is_close_tag(search_tag))
               cout << ":Close";
            cout << endl;
         }
      }

      input_string++;
   }
}

int main(int argc, char **argv)
{
   ifstream input_file(argv[1]);
   string      line;
   const char *line_str;

   while(getline(input_file, line))
   {
      line_str = nhpc_strtolower((char *)line.c_str());
      _search_parser_get_tags(line_str);
   }
}
