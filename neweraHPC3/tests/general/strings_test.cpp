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

#include <include/strings.h>

using namespace std;
using namespace neweraHPC;

int main()
{
   nhpc_status_t nrv;
   
   nrv = nhpc_strcmp("GET", "GET");
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;
   
   nrv = nhpc_strcmp("GET abc", "GET");
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;

   nrv = nhpc_strcmp("GET abc", "GET*");
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;

   nrv = nhpc_strcmp("GET abc", "GET*a*b*c*");
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;

   nrv = nhpc_strcmp("GET abc", "*SET*a*b*c");
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;
   
   return 0;
}
