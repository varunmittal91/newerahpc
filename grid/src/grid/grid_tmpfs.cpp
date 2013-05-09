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

#include <sys/stat.h>
#include <sys/errno.h>
#include <iostream>

#include <neweraHPC/neweraHPC.h>

#include <include/grid_server.h>

using namespace std;

namespace neweraHPC
{
   void grid_tmpfs_init()
   {
      _tmp_dir = NULL;
      
      const char *tmp_path = nhpc_get_cmdline_argument("tmpfs");
      if(!tmp_path)
	 tmp_path = "/tmp/neweraHPC_grid";
      
      int rv = mkdir(tmp_path, 0777);
      if(rv == 0 || errno == EEXIST)
      {
	 nhpc_strcpy((char **)&_tmp_dir, tmp_path);
      }
   }
   
   nhpc_status_t grid_tmpfs_mkdir(const char **final_path, const char *dest)
   {
      if(!_tmp_dir)
	 return NHPC_FAIL;
      
      const char *parent_path = _tmp_dir;
      const char *_path       = dest;
      
      nhpc_status_t nrv;      
      if(_path)
      {
	 while(*_path == '.' || *_path == '/')
	    _path++;

	 nhpc_strcpy((char **)&_path, _path);
      }
      else 
	 _path = nhpc_random_string(9);
      
      nrv = nhpc_create_file(final_path, parent_path, _path);
      cout << parent_path << "/" << _path << endl;
      
      cout << nrv << endl;
      
      int i = 0;
      char *_tmp_path = NULL;
      char *i_str;
      while(nrv == EEXIST)
      {
	 i_str     = nhpc_itostr(i);
	 _tmp_path = nhpc_strconcat(_path, ".", i_str);

	 nrv = nhpc_create_dir(final_path, parent_path, _tmp_path);	 
	 
	 delete[] _tmp_path;
	 delete[] i_str;
	 i++;
      }
      delete[] _path;
      
      return NHPC_FAIL;
   }
};