//Copyright 2009 Varun Mittal ©
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

#include <init.h>
#include <iostream>

using namespace std;

namespace newera_mpi{
  newera_hpc_data *devide(newera_hpc_data *n1,newera_hpc_data *n_orig,float *quot){
    newera_hpc_data *n2 = init_data();
    copy(n_orig,n2);
    int *count = &(n1->count);
    *quot = 0;
    if(check_zero(n1)==0){
        delete_data(n2);
        return n1;
    }
    int a = compare(n1,n2);
    int ab = compare(n1,n_orig);
    float temp = 1;
//  while(ab==-1){
    while(1){
       if(n1->digits_used==n2->digits_used)break;
       while(n1->digits_used!=(n2->digits_used-1)){
          multiply(n2,10);
          temp = temp*10;
       }
       *quot = *quot + temp;
       temp = 1;
       subtract(n1,n2);
       re_init_data(n2);
       copy(n_orig,n2); 
       ab = compare(n1,n_orig);
    }
    copy(n_orig,n2);
    while(a!=1){
      add(n2,n_orig);
      a = compare(n1,n2);
      *quot = *quot+1;
      if(a==0){
        re_init_data(n1);
        *quot = *quot+1;
        delete_data(n2);
        return n1;
      }
    }
    subtract(n2,n1);
    cout<<"a"<<endl;
    print_f(n2);
    cout<<"a"<<endl;
    re_init_data(n1);
    copy(n_orig,n1);
    subtract(n1,n2);
    delete_data(n2);
    return n1;
  }
};
