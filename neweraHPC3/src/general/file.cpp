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

#include <sys/stat.h>

#include <include/file.h>

namespace neweraHPC
{
   nhpc_status_t nhpc_file_size(const char *file_path, nhpc_size_t *size)
   {
      struct stat *buffer = new struct stat;
      nhpc_size_t *tmp_size = new nhpc_size_t;
      
      nhpc_status_t rv = stat(file_path, buffer);
      if(rv != 0)
      {
	 *tmp_size = 0;
	 rv = NHPC_FILE_NOT_FOUND;
      }
      else if(S_ISDIR (buffer->st_mode))
      {
	 *tmp_size = 0;
	 rv = NHPC_DIRECTORY;
      }
      else 
      {
	 *tmp_size = buffer->st_size;
	 rv = NHPC_FILE;
      }
      
      if(size != NULL)
	 *size = *tmp_size;
      
      delete tmp_size;
      delete buffer;
      return rv;
   }
   
   nhpc_status_t nhpc_filecopy(const char *dst, const char *src)
   {
      if(dst == NULL || src == NULL)
	 return NHPC_FAIL;
      
      if(nhpc_file_size(dst, NULL) != NHPC_FILE_NOT_FOUND || nhpc_file_size(src, NULL) != NHPC_FILE)
	 return NHPC_FAIL;
      
      FILE *fp_src = fopen(src, "r");
      if(!fp_src)
	 return NHPC_FAIL;
      
      FILE *fp_dst = fopen(dst, "w+");
      if(!fp_dst)
      {
	 fclose(fp_src);
	 return NHPC_FAIL;
      }
      
      nhpc_size_t size;
      char buffer[1000];
      while(!feof(fp_src))
      {
	 size = fread(buffer, 1, sizeof(buffer), fp_src); 
	 fwrite(buffer, 1, size, fp_dst);
      }
      
      fclose(fp_src);
      fclose(fp_dst);
      
      return NHPC_SUCCESS;
   }
};