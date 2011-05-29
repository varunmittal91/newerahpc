/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
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

#include "rbtree.h"
#include <iostream>

using namespace std;
using namespace neweraHPC;

int main(){
	rbtree test_tree;

	int *a,*b,*c,*d;
	a = new int;
	b = new int;
	c = new int;
	d = new int;
	
	cout<<a<<endl;
	cout<<b<<endl;
	cout<<c<<endl;
	cout<<d<<endl;

	cout<<test_tree.insert(a)<<endl;
	cout<<test_tree.insert(b)<<endl;
	cout<<test_tree.insert(c)<<endl;
	cout<<test_tree.insert(d)<<endl;

}