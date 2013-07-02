/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <include/neweraHPC.h>

nhpc_int_t nhpc_strcmp(const char *s1, const char *s2) {
   const char *tmp_s1 = s1;
   const char *tmp_s2 = s2;
   bool loop = false;
   const char *tmp_loop_s2;
   
   if(*tmp_s2 == '*')
      while(*(tmp_s2 + 1) == '*')
	 tmp_s2++;
   
   if(*tmp_s2 == '\0')
      return NHPC_SUCCESS;
   
   while(*tmp_s1 != '\0' || *tmp_s2 != '\0') {
      if(*tmp_s1 == *tmp_s2) {
	 tmp_s1++;
	 tmp_s2++;
	 continue;
      }
      else if(*tmp_s1 == '\0' && *tmp_s2 == '*') {
	 while(*tmp_s2 == '*')
	    tmp_s2++;
	 continue;
      }
      else if(*tmp_s2 == '*') {
	 while(*(tmp_s2 + 1) == '*')
	    tmp_s2++;
	 
	 loop = true;
	 tmp_loop_s2 = tmp_s2;
	 
	 while(*tmp_s1 != *(tmp_s2 + 1) && *tmp_s1 != '\0') {
	    *tmp_s1++;
	 }
	 
	 *tmp_s2++;
      }
      else {
	 if(loop != true) {
	    return ((nhpc_int_t)*tmp_s1 - (nhpc_int_t)*tmp_s2);
	 }
	 else {
	    loop = false;
	    tmp_s2 = tmp_loop_s2;
	 }
      }
   }
   
   return NHPC_SUCCESS;
}

char *nhpc_itoa(char *dst, int num) {
   if(num <= 0) {
      dst[0] = '0';
      dst[1] = '\0';
      
      return dst;
   }
   
   int count = 0;
   
   int tmp_num = 1;
   while(num > tmp_num)
   {
      tmp_num = tmp_num * 10;
      count++;
   }
   
   for(int i = 1; i <= count; i++)
   {
      dst[count - i] = (num % 10) + 48;
      num = num / 10;
   }
   dst[count] = '\0';

   return dst;
}
