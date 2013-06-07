/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com>
 *	jarvis program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of jarvis.
 *
 *	jarvis is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	jarvis is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with jarvis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <neweraHPC/strings.h>
#include <neweraHPC/error.h>

using namespace neweraHPC;

namespace jarvis
{
   void jv_get_record_parts(char *record_string, char **record_parts, int *count)
   {
      int   i = 0;
      int   space_pos;
      char *tmp_string;
      
      skip_white_spaces(&record_string);
      while((space_pos = nhpc_strfind(record_string, ' ')))
      {
	 if(*count < i)
	 {
	    LOG_ERROR("record parts smaller than required, required size:" << i);
	    return;
	 }
	 
	 record_string[space_pos - 1] = '\0';
	 tmp_string = record_string;
	 record_parts[i] = tmp_string;
	 record_string += space_pos; 

	 skip_white_spaces(&record_string);
	 
	 i++;
      }
      
      if(*(record_string) != '\0')
      {
	 record_parts[i] = record_string;
	 i++;
      }

      (*count) = i;
   }
};