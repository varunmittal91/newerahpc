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
	tree::tree(){
		node = NULL;
		count = 0;
	}
	tree::~tree(){
		
	}
	void tree::operator+=(void *in_p){
		count++;
		tree_elem *new_elem = (tree_elem *)malloc(sizeof(tree_elem));
		new_elem->data = in_p;
		new_elem->left = NULL;
		new_elem->right = NULL;
		if(node==NULL){
			node = new_elem;
			return;
		}
		tree_elem *tmp_elem = node;	
		while(tmp_elem->left!=NULL||tmp_elem->right!=NULL){
			if(in_p>=tmp_elem->data){
				if(tmp_elem->left==NULL)break;
				tmp_elem = tmp_elem->left;
			}
			else{
				if(tmp_elem->left==NULL)break;
				tmp_elem = tmp_elem->right;
			}
		}
		if(in_p>=tmp_elem->data){
			tmp_elem->left = new_elem;
		}
		else{
			tmp_elem->right = new_elem;
		}
	}
	void tree::printTree(tree_elem* st_node=NULL){
		if (st_node == NULL){
			if(node==NULL)return;
			printTree(node);
			return;
		}
		if(st_node->left!=NULL){
			printTree(st_node->left);
			cout<<"first elem is left"<<endl;
		}
		cout<<st_node->data<<" ";
		if(st_node->right!=NULL){
			printTree(st_node->right);
			cout<<"first elem is right"<<endl;
		}
	}
};