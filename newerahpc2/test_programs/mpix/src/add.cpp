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
  void add(newera_hpc_data *n1,newera_hpc_data *n2){
    int limit;
    newera_hpc_bit *num1= n1->data;
    newera_hpc_bit *num2= n2->data;
    if(n1->digits_used>=n2->digits_used){
       limit = (n2->digits_used);
    }
    else{
       limit = (n1->digits_used);
    }
    int temp_c3;
    int temp_c4=0;
    for(temp_c3=(n1->count-1);temp_c3>=limit;temp_c3--){
        temp_c4++;
        num1[temp_c3]=num1[temp_c3]+num2[temp_c3];
        if(num1[temp_c3]>9){
           num1[temp_c3]=num1[temp_c3]%10;
           num1[temp_c3-1]++;
           if(temp_c3==limit){
              (n1->digits_used)=limit;
              (n1->digits_used)--;
           }
        }
        else if(temp_c3==limit){
           if(n1->digits_used>n2->digits_used)
             (n1->digits_used)=limit;
        }
    }
    if(limit==0)limit=1;
    num1[limit-1]=num1[limit-1]+num2[limit-1];
  }
};
