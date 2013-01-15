/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
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

#include <iostream>
#include <math.h>
#ifdef linux
#include <stdlib.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <include/strings.h>
#include <include/strings_pool.h>
#include <include/constants.h>
#include <include/error.h>
#include <include/alloc.h>

using namespace std;

namespace neweraHPC{ 
   const char *alphabets = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   
   nhpc_status_t nhpc_strcpy(char **dst, const char *src)
   {
      nhpc_size_t len = strlen(src);
      
      (*dst) = new char [len + 1];
      memcpy(*dst, src, len);
      (*dst)[len] = '\0';
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_strcpy_noalloc(char *dst, const char *src)
   {
      nhpc_size_t len = strlen(src);
      
      memcpy(dst, src, len);
      dst[len] = '\0';
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_strcmp(const char *s1, const char *s2)
   {
      const char *tmp_s1 = s1;
      const char *tmp_s2 = s2;
      bool loop = false;
      const char *tmp_loop_s2;
      
      if(*tmp_s2 == '*')
	 while(*(tmp_s2 + 1) == '*')
	    tmp_s2++;
      
      if(*tmp_s2 == '\0')
	 return NHPC_SUCCESS;
      
      while(*tmp_s1 != '\0' || *tmp_s2 != '\0')
      {
	 if(*tmp_s1 == *tmp_s2)
	 {
	    tmp_s1++;
	    tmp_s2++;
	    continue;
	 }
	 else if(*tmp_s1 == '\0' && *tmp_s2 == '*')
	 {
	    while(*tmp_s2 == '*')
	       tmp_s2++;
	    continue;
	 }
	 else if(*tmp_s2 == '*')
	 {
	    while(*(tmp_s2 + 1) == '*')
	       tmp_s2++;
	    
	    loop = true;
	    tmp_loop_s2 = tmp_s2;
	    
	    while(*tmp_s1 != *(tmp_s2 + 1) && *tmp_s1 != '\0')
	    {
	       *tmp_s1++;
	    }
	    
	    *tmp_s2++;
	 }
	 else 
	 {
	    if(loop != true)
	       return NHPC_FAIL;
	    else 
	    {
	       loop = false;
	       tmp_s2 = tmp_loop_s2;
	    }
	 }
      }
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t nhpc_strcmpi(const char *s1, const char *s2)
   {
      const char *tmp_s1 = s1;
      const char *tmp_s2 = s2;
      bool loop = false;
      const char *tmp_loop_s2;
      
      if(*tmp_s2 == '*')
	 while(*(tmp_s2 + 1) == '*')
	    tmp_s2++;
      
      if(*tmp_s2 == '\0')
	 return NHPC_SUCCESS;
      
      while(*tmp_s1 != '\0' || *tmp_s2 != '\0')
      {
	 if(tolower(*tmp_s1) == tolower(*tmp_s2))
	 {
	    tmp_s1++;
	    tmp_s2++;
	    continue;
	 }
	 else if(*tmp_s1 == '\0' && *tmp_s2 == '*')
	 {
	    while(*tmp_s2 == '*')
	       tmp_s2++;
	    continue;
	 }
	 else if(*tmp_s2 == '*')
	 {
	    while(*(tmp_s2 + 1) == '*')
	       tmp_s2++;
	    
	    loop = true;
	    tmp_loop_s2 = tmp_s2;
	    
	    while(tolower(*tmp_s1) != tolower(*(tmp_s2 + 1)) && *tmp_s1 != '\0')
	    {
	       *tmp_s1++;
	    }
	    
	    *tmp_s2++;
	 }
	 else 
	 {
	    if(loop != true)
	       return NHPC_FAIL;
	    else 
	    {
	       loop = false;
	       tmp_s2 = tmp_loop_s2;
	    }
	 }
      }
      
      return NHPC_SUCCESS;
   }
   
   int nhpc_strfind(const char *s1, const char s2)
   {
      return nhpc_strfind(s1, s2, 1);
   }
   
   int nhpc_strfind(const char *s1, const char s2, int old_position)
   {
      const char *tmp_s1 = s1;
      int position = 0;
      
      old_position--;
      while(*tmp_s1 != '\0' && old_position != 0)
      {
	 tmp_s1++;
	 old_position--;
	 position++;
      }
      
      if(old_position > 0)
      {
	 position = 0;
	 return position;
      }
      
      while(*tmp_s1 != '\0')
      {
	 position++;
	 if(*tmp_s1 == s2)
	 {
	    return position;
	 }	 
	 tmp_s1++;
      }
      
      position = 0;
      return position;
   }
   
   char *nhpc_substr(const char *s1, int start_pos, int end_pos)
   {
      if(start_pos < 1 || end_pos < 1 || end_pos > strlen(s1))
	 return NULL;
      
      nhpc_size_t len = end_pos - start_pos + 2;
      char *new_string = new char[len];
      memcpy(new_string, (s1 + start_pos - 1), len - 1);
      new_string[len - 1] = '\0';
      
      return new_string;
   }
   
   string_t *nhpc_substr(const char *s1, const char s2)
   {      
      string_t  *string      = new string_t;
      int       *count       = &(string->count);
      char     **strings     = NULL;
      char     **tmp_strings = NULL;

      const char *tmp_s1 = s1;
      
      int tmp_len = 0;
      
      while(*tmp_s1 != '\0')
      {
	 if(*tmp_s1 == s2)
	 {
	    if(tmp_len > 0)
	    {
	       char *new_string = new char [tmp_len + 1];
	       memcpy(new_string, (tmp_s1 - tmp_len), tmp_len);
	       new_string[tmp_len] = '\0';
	       (*count)++;
	       
	       if(!strings)
	       {
		  strings = new char* [2];
		  strings[0] = new_string;
		  strings[1] = NULL;
	       }
	       else 
	       {
		  tmp_strings = strings;
		  strings = new char* [*count + 1];
		  memcpy(strings, tmp_strings, (sizeof(char*) * (*count - 1)));
		  strings[*count - 1] = new_string;
		  strings[*count] = NULL;
		  delete[] tmp_strings;
	       }
	    }
	    
	    tmp_len = -1;
	 }
	 
	 tmp_len++;
	 tmp_s1++;
      }
      
      if(tmp_len > 0)
      {
	 char *new_string = new char[tmp_len + 1];
	 memcpy(new_string, (tmp_s1 - tmp_len), tmp_len);
	 new_string[tmp_len] = '\0';
	 (*count)++;
	 
	 if(!strings)
	 {
	    strings = new char* [2];
	    strings[0] = new_string;
	    strings[1] = NULL;
	 }
	 else 
	 {
	    tmp_strings = strings;
	    strings = new char* [*count + 1];
	    memcpy(strings, tmp_strings, (sizeof(char*) * (*count - 1)));
	    strings[*count - 1] = new_string;
	    strings[*count] = NULL;
	    delete[] tmp_strings;
	 }	 
      }
      
      string->strings = strings;
      
      return string;
   }
   
   void nhpc_string_delete(string_t *string)
   {
      for(int i = 0; i < string->count; i++)
      {
	 delete[] (string->strings[i]);
      }
      
      delete[] (string->strings);
      
      delete string;
   }
   
   void nhpc_string_delete(char *string)
   {
      delete[] string;
   }
   
   char *nhpc_strconcat_va(const char *fmt, ...)
   {
      nhpc_size_t len = 0;

      const char *s;
      char       *string = NULL;      
      va_list     ap;
      
      va_start(ap, fmt);      
      while((s = va_arg(ap, const char *)))
      {
	 len += strlen(s);
      }
      va_end(ap);
      
      if(len == 0)
      {
	 return NULL;
      }
      string = new char [len + 1];
      string[len] = '\0';

      char  *tmp = string;
      
      va_start(ap, fmt);
      while((s = va_arg(ap, const char *)))
      {
	 len = strlen(s);
	 memcpy((void *)tmp, s, len * sizeof(char));
	 tmp += len;
      }      
      va_end(ap);
      
      return string;
   }

   char *nhpc_itostr(int num)
   {
      int tmp_num = num;
      int count = 0;
      
      if(num <= 0)
      {
	 char *string = new char [2];
	 string[0] = '0';
	 string[1] = '\0';
	 return string;
      }
      
      while(tmp_num != 0)
      {
	 int digit = tmp_num % 10;
	 tmp_num = tmp_num / 10;	 
	 count++;
      }
      
      tmp_num = num;
      char *string = new char [count + 1];
      string[count] = '\0';
      int i = 0;
      
      while(tmp_num != 0)
      {
	 int digit = tmp_num % 10;
	 tmp_num = tmp_num / 10;
	 string[count - i - 1] = digit + 48;
	 i++;
      }      
      
      return string;
   }
   
   int nhpc_hexstrtoi(const char *str)
   {
      const char *tmp_str = str;
      int size = strlen(str);
      int num = 0;
      
      int tmp_num = 0;
      int ascii_value;
      while(*tmp_str != '\0')
      {
	 ascii_value = (int)(*tmp_str);
	 if(ascii_value >=97 && ascii_value <= 102)
	    tmp_num = ascii_value - 87;
	 else 
	    tmp_num = ascii_value - 48;
	 
	 num += tmp_num * pow(16, (size - 1));
	 size--;
	 tmp_str++;
      }
      
      return num;
   }
   
   int nhpc_strtoi(const char *str)
   {
      const char *tmp_str = str;
      int size = strlen(str);
      int num = 0;
      
      int tmp_num = 0;
      while(*tmp_str != '\0')
      {
	 tmp_num = (int)(*tmp_str) - 48;
	 num += tmp_num * pow(10 ,(size - 1));
	 
	 size--;
	 tmp_str++;
      }
      
      return num;
   }
   
   char *nhpc_random_string(nhpc_size_t len)
   {
      char *dst = new char [len + 1];
      
      for(int i = 0; i < len; i++)
      {
	 time_t ct;
	 time(&ct);
	 
	 srand(rand() + ct);
	 dst[i] = alphabets[rand() % 52];
      }
      
      dst[len] = '\0';
      
      return dst;
   }
   
   int skip_white_spaces(char **string_ptr)
   {
      int skipped_steps = 0;
      
      while(**string_ptr == ' ')
      {
	 skipped_steps++;
	 (*string_ptr)++;
      }
      
      return skipped_steps;
   }
   
   char *nhpc_strtolower(char *s1)
   {
      for(int i = 0; s1[i]; i++)
	 s1[i] = tolower(s1[i]);
      
      return s1;
   }
}
