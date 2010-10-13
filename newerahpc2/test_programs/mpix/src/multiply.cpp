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

#include <data.h>
#include <basic_math.h>
#include <iostream>

using namespace std;

namespace newera_mpi{
  void multiply(newera_hpc_data *n1,newera_hpc_bit num){
    int *limit = &(n1->digits_used);
    int *count = &(n1->count);
    newera_hpc_bit *result = n1->data;
    newera_hpc_bit temp_q=0,temp_r=0;
    for(int a=(*count-1);a>=*limit;a--){
      result[a]=result[a]*num;
      result[a]=result[a]+temp_q;
      if(result[a]<=9){
        temp_r = 0;
        temp_q = 0;
      }
      else{
         temp_r = result[a]%10;
         temp_q = (result[a]/10);
         result[a] = temp_r;
      }
    }
    int temp_c1=*limit-1;
    while(1){
       if(temp_q==0)break;
       if(temp_c1<0)break;
       if(temp_q<10){
          result[temp_c1]=temp_q;
          n1->digits_used = temp_c1;
          break;
       }
       result[temp_c1]=temp_q%10;
       temp_q=temp_q/10;
       (n1->digits_used)--;
       if(temp_q==0)break;
       temp_c1--;
    }
  }
};
