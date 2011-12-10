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

#include <iostream>
#include <stdio.h>
#ifdef linux
#include <stdlib.h>
#endif

#include <include/rbtree.h>

using namespace std;
using namespace neweraHPC;

void func_sleep(void)
{
   while(1)sleep(60);
}

int main()
{   
   cout<<"This program is mainly build built for testing the efficiency of rbtree_t module used in the neweraHPC library"<<endl;
   cout<<"Please use some tool to test for memory leaks\n"<<"Attach the binary to some tool and then press any key"<<endl;
   
   rbtree_t *test_tree = new rbtree_t;
   
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
   delete[] a; 
   delete[] b; 
   delete[] c; 
   delete[] d;
   
   cout<<"All tree structures and data have been deleted there should not be any trace of memory still remaining\n";
   cout<<"If there is still some memory left that is not deallocated, we have a problem"<<endl;
   
   atexit(func_sleep);
   
   return 0;
}
