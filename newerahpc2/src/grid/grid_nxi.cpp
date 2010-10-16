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
	void plugin_manager::read_nxi(const char *file){
		ifstream plg_inp_file(file);
        string out_dir = return_file();
        string tmp = "mkdir " + out_dir;
        system(tmp.c_str());
        string out_exec = out_dir + "/exec";
        ofstream plg_exec(out_exec.c_str());
        tmp = "chmod a+x " + out_dir + "/exec";
        system(tmp.c_str());
        plg_exec<<"#!/bin/sh"<<endl;
        plg_exec<<"cd "<<out_dir<<endl;
        string line;
        fstream out_files;
        while(getline(plg_inp_file,line)){
			if(line.find("build-command")!=string::npos){
				size_t tmp_pos1 = line.find(":");
				plg_exec<<line.substr(tmp_pos1+1,line.length()-tmp_pos1-1)<<endl;
				continue;
			}
			else if(line.find("<<file:over>>")!=string::npos){
				out_files.close();
				continue;
			}
			else if(line.find("<<file:")!=string::npos){
				size_t tmp_pos1 = line.find(":");
				size_t tmp_pos2 = line.find("/");
				string file_name = line.substr(tmp_pos1+1,line.length()-tmp_pos1-3);
				file_name = out_dir + "/" + file_name;
				string in_dir;
				while(tmp_pos2!=string::npos){
					in_dir = line.substr(tmp_pos1+1,tmp_pos2-tmp_pos1-1);
					in_dir = "mkdir " + out_dir + "/" + in_dir;
					system(in_dir.c_str());
					tmp_pos2 = line.find("/",tmp_pos2+1);
				}
				out_files.open(file_name.c_str(),ios::out);
				continue;
			}
			out_files<<line<<endl;
        }
        tmp = out_dir + "/exec";
	}
	void plugin_manager::create_nxi(const char *file){
		string plg_info_path = file;
		size_t pos_t1 = plg_info_path.find("/");
        size_t pos_t2 = pos_t1;
        while(pos_t1!=string::npos){
			pos_t2 = pos_t1;
			pos_t1 = plg_info_path.find("/",pos_t1+1);
        }
        if(pos_t2==string::npos)pos_t2 = 0;
        string plg_info_root = plg_info_path.substr(0,pos_t2);
        char *load_file = (char *)"grid_plugin/plugin.info";
        ifstream plg_info(load_file);
        ofstream plg_out("plg.nxi");
        string line;
        bool file_mode = false;
        while(getline(plg_info,line)){
			if(file_mode!=true){
				if(line.find("build-command")!=string::npos){
					plg_out<<line<<endl;
				}
				else if(line.find("files:")!=string::npos){
					file_mode=true;
				}
				continue;
			}
			string file_name = plg_info_root + "/" + line;
			ifstream plg_file(file_name.c_str());
			if(!plg_file.is_open()){
				ifstream plg_info(load_file);
				ofstream plg_out("plg.nxi");
				string line;
				bool file_mode = false;
				while(getline(plg_info,line)){
					if(file_mode!=true){
                        if(line.find("build-command")!=string::npos){
							plg_out<<line<<endl;
                        }
                        else if(line.find("files:")!=string::npos){
							file_mode=true;
                        }
                        continue;
					}
					string file_name = plg_info_root + "/" + line;
					ifstream plg_file(file_name.c_str());
					if(!plg_file.is_open()){
                        cout<<"file not found"<<endl;
                        return;
					}
					string line_file;
					plg_out<<"<<file:"<<line<<">>"<<endl;
					while(getline(plg_file,line_file)){
                        plg_out<<line_file<<endl;
					}
					plg_out<<"<<file:over>>"<<endl;
				}				
			}
		}
	}
};
		
		
