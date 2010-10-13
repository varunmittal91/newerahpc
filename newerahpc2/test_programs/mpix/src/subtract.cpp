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
  void subtract(newera_hpc_data *n1,newera_hpc_data *n2){
    int *limit;
    int *count = &(n1->count);
    newera_hpc_bit *num1 = n1->data;
    newera_hpc_bit *num2 = n2->data;
    if(n1->digits_used<=n2->digits_used)limit = &(n1->digits_used);
    else limit = &(n2->digits_used);
    for(int temp_c3=(*count-1);temp_c3>=*limit;temp_c3--){
        if(num2[temp_c3]>num1[temp_c3]){
            num1[temp_c3]+=10;
            int temp_c4 = temp_c3-1;
            while(num1[temp_c4]==0){
                if(temp_c4!=0)num1[temp_c4]=9;
                temp_c4--;
                if(temp_c4<0)break;
            }
            if(temp_c4<0)temp_c4=0;
               num1[temp_c4]--;
        }
        num1[temp_c3]=num1[temp_c3]-num2[temp_c3];
    }
    num1[0]=num1[0]-num2[0];
    for(int temp_c3=*limit;temp_c3<(*count-1);temp_c3++){
       if(num1[temp_c3]!=0)break;
       n1->digits_used = n1->digits_used+1;
    }
  }
};
