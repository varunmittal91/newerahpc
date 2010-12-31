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

namespace newera_network{
	void sig_handler(int signal){
		if(signal==SIGPIPE);
		else if(signal==SIGINT){
			cout<<"Exit function called"<<endl;
			stop_log();
			delete local_rec;
			delete database;
			delete hpc_data;
			delete mem_obj;
			exit(0);
		}
	}
};
