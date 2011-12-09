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
   const char *s1;
   const char *s2;
   
   s1 = "GET";
   s2 = "GET";
   cout<<"s1: "<<s1<<" s2: "<<s2<<endl;
   nrv = nhpc_strcmp(s1, s2);
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;
   
   s1 = "GET abc";
   s2 = "GET";
   cout<<"s1: "<<s1<<" s2: "<<s2<<endl;
   nrv = nhpc_strcmp(s1, s2);
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;

   s1 = "GET abc";
   s2 = "GET*";
   cout<<"s1: "<<s1<<" s2: "<<s2<<endl;
   nrv = nhpc_strcmp(s1, s2);
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;

   s1 = "ASB GET abc HTTP/1.1";
   s2 = "*GET";
   cout<<"s1: "<<s1<<" s2: "<<s2<<endl;
   nrv = nhpc_strcmp(s1, s2);
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;

   s1 = "GET abc";
   s2 = "**GET*a***bd***c**";
   cout<<"s1: "<<s1<<" s2: "<<s2<<endl;
   nrv = nhpc_strcmp(s1, s2);
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;
   
   s1 = "";
   s2 = "";
   cout<<"s1: "<<s1<<" s2: "<<s2<<endl;
   nrv = nhpc_strcmp(s1, s2);
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;

   s1 = "Varun Dhawan";
   s2 = "*varun*";
   cout<<"s1: "<<s1<<" s2: "<<s2<<endl;
   nrv = nhpc_strcmpi(s1, s2);
   if(nrv == NHPC_FAIL)
      cout<<"string not found"<<endl;
   else
      cout<<"string found"<<endl;
   cout<<endl;
   
   string_t *string = nhpc_substr("My name is   varun", ' ');
   string = nhpc_substr("   My name is   varun", ' ');
   string = nhpc_substr("My name is   varun   ", ' ');
   string = nhpc_substr("My name is varun", 's');
   
   return 0;
}
