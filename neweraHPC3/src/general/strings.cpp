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
#ifdef linux
#include <stdlib.h>
#endif

#include <include/strings.h>
#include <include/constants.h>

using namespace std;

namespace neweraHPC{
   char *nhpc_strcpy(char *dst, const char *src, size_t dst_len)
   {
      char *d, *end;
      
      if(dst_len == 0) 
      {
	 return (dst);
      }
      
      d = dst;
      end = dst + dst_len - 1;
      
      for(; d < end; ++d, ++src)
      {
	 if(!(*d = *src)) 
	 {
	    return (d);
	 }
      }
      
      *d = '\0';
      
      return (d);
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
   
   string_t *nhpc_substr(const char *s1, const char s2)
   {
      string_t *string = NULL;
      
      const char *tmp_s1 = s1;
      int old_pos = 0;
      int current_pos = 1;
      size_t len = 0;
      
      while(*tmp_s1 != '\0')
      {
	 if(*tmp_s1 == s2)
	 {
	    len = current_pos - old_pos - 1;
	    
	    char *tmp_string;
	    if(len != 0)
	    {
	       if(string == NULL)
	       {
		  string = new string_t;
		  string->strings = new char*;
		  string->strings[0] = new char [len + 1];
		  tmp_string = string->strings[0];
		  string->count = 1;
	       }
	       else 
	       {
		  char **tmp_strings = new char* [string->count + 1];
		  memcpy(tmp_strings, string->strings, sizeof(char*)*(string->count));
		  delete string->strings;
		  string->strings = tmp_strings;
		  string->strings[string->count] = new char [len + 1];
		  tmp_string = string->strings[string->count];
		  (string->count)++;
	       }
	       
	       memcpy(tmp_string, (tmp_s1 - len), len);
	       tmp_string[len] = '\0';
	    }
	    
	    old_pos = current_pos;
	    
	    while(*(tmp_s1 + 1) == s2)
	    {
	       tmp_s1++;
	       current_pos++;
	       old_pos++;
	    }
	 }
	 
	 current_pos++;
	 tmp_s1++;
      }
      
      if(old_pos != 0)
      {
	 len = (current_pos - old_pos -1);
	 
	 if(len != 0)
	 {
	    char **tmp_strings = new char* [string->count + 1];
	    memcpy(tmp_strings, string->strings, sizeof(char*)*(string->count));
	    delete string->strings;
	    string->strings = tmp_strings;
	    string->strings[string->count] = new char [len + 1];
	    char *tmp_string = string->strings[string->count];
	    (string->count)++;	 
	    
	    memcpy(tmp_string, (tmp_s1 - len), len);
	    tmp_string[len] = '\0';
	 }
      }
      
      return string;
   }
   
   char *nhpc_strconcat(const char *s1, const char *s2)
   {
      char *string = NULL;
      
      if(s1 == NULL || s2 == NULL)
	 return NULL;
      
      nhpc_size_t len_s1 = strlen(s1);
      nhpc_size_t len_s2 = strlen(s2);
      
      if((len_s1 == 0) && (len_s2 == 0))
	 return NULL;
      else if(len_s1 == 0)
      {
	 string = new char [len_s2 + 1];
	 memcpy(string, s2, len_s2);
	 string[len_s2] = '\0';
      }
      else if(len_s2 == 0)
      {
	 string = new char [len_s1 + 1];
	 memcpy(string, s1, len_s1);
	 string[len_s1] = '\0';
      }
      else 
      {
	 nhpc_size_t len = strlen(s1) + strlen(s2);

	 string = new char [len + 1];
	 memcpy(string, s1, len_s1);
	 memcpy(string + len_s1, s2, len_s2);
	 string[len] = '\0';
      }

      return string;
   }
   
   char *nhpc_itostr(int num)
   {
      int tmp_num = num;
      int count = 0;
      
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
}
