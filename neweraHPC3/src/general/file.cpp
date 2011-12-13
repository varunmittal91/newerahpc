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
   nhpc_size_t nhpc_file_size(const char *file_path, nhpc_size_t *size)
   {
      struct stat *buffer = new struct stat;
      nhpc_status_t rv = stat(file_path, buffer);
      if(rv != 0)
      {
	 *size = 0;
	 rv = NHPC_FILE_NOT_FOUND;
      }
      else if(S_ISDIR (buffer->st_mode))
      {
	 *size = 0;
	 rv = NHPC_DIRECTORY;
      }
      else 
      {
	 *size = buffer->st_size;
	 rv = NHPC_FILE;
      }
      
      delete buffer;
      return rv;
   }
};