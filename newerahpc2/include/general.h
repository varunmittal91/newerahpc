//Copyright 2010 Varun Mittal ©
//newerahpc program is distributed under the terms of the GNU General Public License
//
//This file is part of newerahpc.
//
//newerahpc is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation version 2 of the License.
//
//newerahpc is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with newerahpc.  If not, see <http://www.gnu.org/licenses/>.

#define STR_NPOS -1
#define FILE_T 1
#define DIR_T 2

namespace newera_network{
	extern std::ofstream *log_file;
	size_t find(char *,char *);
	size_t find(char *,char *,size_t);
	char *substr(char *,size_t,size_t);
	char *itoa(int);
	std::string return_file();
	std::string return_file(int);
	int get_file_size(char *);
	void init_log();
	void add_log(const char *);
};
