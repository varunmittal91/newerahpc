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
#include <time.h>

namespace newera_network{
#ifdef debug
	std::ofstream *log_file;
#endif
	void init_log(){
		log_file = new ofstream("log_file");
	}
	void stop_log(){
#ifdef debug
		(*log_file).close();
		delete log_file;
#endif					
	}
	void add_log(const char *str){
		time_t rawtime;
		time(&rawtime);
		(*log_file)<<ctime(&rawtime)<<" "<<str<<endl;
	}
};

