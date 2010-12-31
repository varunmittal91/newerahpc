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

#include <network.h>

using namespace std;

namespace newera_network{
	string return_file(){
		int stat = 0;
		int count=0;
		string file;
		while(stat!=1){
			file = "/tmp/temp";
			file += itoa(count);
			ifstream temp(file.c_str());
			if(!temp.is_open())stat=1;
			count++;
		}
		return file;
	}
	string return_file(int type){
		int stat = 0;
		int count=0;
		string file;
		while(stat!=1){
			file = "/tmp/temp";
			file += itoa(count);
			ifstream temp(file.c_str());
			if(!temp.is_open()){
				stat=1;
				temp.close();
			}
			count++;
		}
		string cmd;
		if(type==FILE_T){
			cmd = "touch "+file;
		}
		else{
			cmd = "mkdir "+file;
		}
		system(cmd.c_str());
		return file;
	}
	int get_file_size(char *file){
		int file_size;
		ifstream data(file);
		data.seekg(0,ios::end);
		file_size = data.tellg();
		return file_size;
	}
};
