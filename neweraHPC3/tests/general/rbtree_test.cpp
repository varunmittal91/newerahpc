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

#include <include/rbtree.h>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace neweraHPC;

int main(){
   
   cout<<"This program is mainly build built for testing the efficiency of rbtree module used in the neweraHPC library"<<endl;
   cout<<"Please use some tool to test for memory leaks\n"<<"Attach the binary to some tool and then press any key"<<endl;
   
   rbtree *test_tree = new rbtree;
   
   int *a,*b,*c,*d;
   a = new int[122];
   b = new int[122];
   c = new int[122];
   d = new int[122];
   
   cout<<a<<endl;
   cout<<b<<endl;
   cout<<c<<endl;
   cout<<d<<endl;
   
   cout<<(*test_tree).insert(a)<<endl;
   cout<<(*test_tree).insert(b)<<endl;
   cout<<(*test_tree).insert(c)<<endl;
   cout<<(*test_tree).insert(d)<<endl;

   delete test_tree;
   delete a,b,c,d;
   
   cout<<"All tree structures and data have been deleted there should not be any trace of memory still remaining\n";
   cout<<"If there is still some memory left that is not deallocated, we have a problem"<<endl;
   
   return 0;
}