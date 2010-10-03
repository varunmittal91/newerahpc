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

//This program is to become the grid's own language
//for dynamic result generation based on the need.

#include <network.h>

namespace newera_network{
	void newera_hpc::grid_lng_init(network_write *write,char *file_path){
		char *test_msg = (char *)"\nthis is grid test message";
		write->add(test_msg);
		ifstream file(file_path);
		string line;
		while(getline(file,line)){
			if(line.find("show connections")!=string::npos){
				char *connections = (char *)"here u will find connection information";
				write->add(connections);
			}
		}
	};

};