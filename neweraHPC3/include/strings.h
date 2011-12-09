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

#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <unistd.h>
#include <string.h>
#include <string>

#include "constants.h"

namespace neweraHPC{
   struct string_t
   {
      char **strings;
      int count;
   };
   
   char *nhpc_strcpy(char *dst, const char *src, size_t dst_len);
   
   /* nhpc_strcmp and nhpc_strcmpi takes two strings as input.
      First string is the subject and the second string is 
      the string to be matched. Second string can have * operator,
      kleen closure.
      First function is case sensitive while other is not. */
   nhpc_status_t nhpc_strcmp(const char *s1, const char *s2);
   nhpc_status_t nhpc_strcmpi(const char *s1, const char *s2);
   
   string_t *nhpc_substr(const char *s1, const char s2);
};

#endif