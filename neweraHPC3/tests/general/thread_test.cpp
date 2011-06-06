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

#include <include/neweraHPC.h>
#include <iostream>

using namespace std;
using namespace neweraHPC;

void *test(void *args){
   cout<<"Hi thread created successfully"<<endl;
}

int main()
{
   thread_manager_t thread_manager;

   int thread_id = thread_manager.create_thread(NULL,test,NULL,THREAD_JOIN);
   if(thread_id==0)
      cout<<"Thread create failed"<<endl;
   
   thread_manager.delete_thread_data(thread_id);
   
   sleep(1);
   return 0;
}
