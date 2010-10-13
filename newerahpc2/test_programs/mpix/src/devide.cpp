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
#include <stdlib.h>

using namespace std;

namespace newera_mpi{
  newera_hpc_data *devide(newera_hpc_data *n1,newera_hpc_data *n2,float *quot){
    int ab = compare(n1,n2);
    if(ab==1){
      *quot = 0;
      return n1;
    }
    newera_hpc_data *n_temp = init_data();
    copy(n2,n_temp);
    int *count = &(n1->count);
    *quot = 0;
    if(check_zero(n1)==0){
        delete_data(n_temp);
        return n1;
    }
    float temp = 1;
    while(1){
       if(n1->digits_used==n_temp->digits_used)break;
       while(n1->digits_used!=(n_temp->digits_used-1)){
          multiply(n_temp,10);
          temp = temp*10;
       }
       *quot = *quot + temp;
       temp = 1;
       subtract(n1,n_temp);
       if(compare(n1,n2)==1)break;
       re_init_data(n_temp);
       copy(n2,n_temp); 
    }
    copy(n2,n_temp);
    int a = compare(n_temp,n1);
    while(a!=-1){
      add(n_temp,n2);
      a = compare(n_temp,n1);
      if(a==0){
        re_init_data(n1);
        *quot = *quot+1;
        delete_data(n_temp);
        return n1;
      }
      else *quot = *quot+1;
    }
    subtract(n_temp,n1);
    copy(n2,n1);
    subtract(n1,n_temp);
    delete_data(n_temp);
    return n1;
  }
};
