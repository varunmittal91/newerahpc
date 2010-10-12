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

#define FILE_REQUEST_DEF "index.html"
#define ROOT "/http_root"
#define SERVER_STRING "newera server 1.0"

namespace newera_network{
	int filedir_check(const char *file){
		FILE *fp;
		fp = fopen(file,"r");
		if(fp==0)return FILE_DIR_NFOUND;
		else{
			fclose(fp);
			DIR *dp = opendir(file);
			if(dp==0)return FILE_FOUND;
			else{
				closedir(dp);
				return DIR_FOUND;
			}
		}
	}
	void http_data::get_file_size(){
		string test = req_details[0];
		ifstream data(test.c_str());
		data.seekg(0,ios::end);
		file_size = data.tellg();
		data.close();
	}
	void http_data::file_request(){
		char *root_cnst = (char *)ROOT;
		size_t length;
		int &status = *&(file_status);
		length = strlen(req_details[0])+strlen(ROOT) + 1;
		char *file_req = (char *)malloc(length);
		bzero(file_req,length);
		memcpy(file_req,root_cnst,strlen(root_cnst));
		memcpy(file_req+strlen(root_cnst),req_details[0],strlen(req_details[0]));
		file_req[length-1] = '\0';
		int file_chk = filedir_check(file_req);
		status = file_chk;
		if(file_chk==FILE_FOUND){
			delete []req_details[0];
			req_details[0] = file_req;
		}
		else if(file_chk==DIR_FOUND){
			char *file_temp;
			if(file_req[strlen(file_req)-1]!='/'){
				length = strlen(file_req)+strlen(FILE_REQUEST_DEF)+1 +1;
				file_temp = (char *)malloc(strlen(file_req)+strlen(FILE_REQUEST_DEF)+1 +1);
				bzero(file_temp,strlen(file_req)+strlen(FILE_REQUEST_DEF)+1 +1);
				strcat(file_temp,file_req);
				strcat(file_temp,"/");
				file_temp[length] = '\0';
			}
			else{
				length = strlen(file_req)+strlen(FILE_REQUEST_DEF) + 1;
				file_temp = (char *)malloc(strlen(file_req)+strlen(FILE_REQUEST_DEF)+1);
				bzero(file_temp,strlen(file_req)+strlen(FILE_REQUEST_DEF)+1);
				strcat(file_temp,file_req);
				file_temp[length] = '\0';
			}
			strcat(file_temp,FILE_REQUEST_DEF);
			file_chk = filedir_check(file_temp);
			status = file_chk;
			free(req_details[FILE_REQUEST]);
			if(file_chk == FILE_FOUND){
				free(file_req);
				req_details[FILE_REQUEST] = file_temp;
			}
			else{
				status_code = 1;
				status = DIR_FOUND;
				req_details[FILE_REQUEST] = file_req;
			}
		}
		if(status == FILE_FOUND){
			get_file_size();
			if(find(req_details[FILE_REQUEST],(char *)".grid")!=STR_NPOS){
			   lng_supp = true;
			}
		}
		else if(status==FILE_DIR_NFOUND){
			status_code = 11;
			free(file_req);
		}
	}
};
