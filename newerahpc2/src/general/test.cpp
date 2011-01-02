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

#include "../../include/network.h"

using namespace newera_network;

int main(){
	tree test_tree;
	int *tmp_elem[3];
	tmp_elem[0] = new int;
	tmp_elem[1] = new int;
	tmp_elem[2] = new int;
	tmp_elem[3] = new int;
	cout<<tmp_elem[0]<<endl;
	cout<<tmp_elem[1]<<endl;
	cout<<tmp_elem[2]<<endl;
	cout<<tmp_elem[3]<<endl;
	test_tree += tmp_elem[3];
	test_tree += tmp_elem[1];
	test_tree += tmp_elem[0];
	test_tree += tmp_elem[2];
	test_tree.printTree(NULL);
	cout<<endl;
	return 0;
}
