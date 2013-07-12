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

#include <sys/stat.h>
#include <errno.h>

#include <include/nhpc_general.h>

nhpc_status_t nhpc_file_size(const char *file_path, nhpc_size_t *size) {
   struct stat buffer;
   
   *size = 0;
   if(stat(file_path, &buffer) == -1)
      return errno;
   else if(S_ISDIR(buffer.st_mode))
      return NHPC_FAIL;
      
   *size = buffer.st_size;
   
   return NHPC_FILE;   
}

nhpc_status_t nhpc_fileordirectory(const char *file_path) {
   struct stat buffer; 
   
   if(stat(file_path, &buffer) == -1)
      return NHPC_FILE_NOT_FOUND;
   else if(!S_ISDIR(buffer.st_mode))
      return NHPC_DIRECTORY;
   else 
      return NHPC_FILE;
}
