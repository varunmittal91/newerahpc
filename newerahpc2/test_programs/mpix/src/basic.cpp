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


#include <iostream>
#include <init.h>

using namespace std;

namespace newera_mpi{
  void assign(newera_hpc_data *input,int num){
    newera_hpc_bit *data = input->data;
    int *count = &(input->count);
    int a=0;
    input->digits_used = input->count;
    while(1){
      data[*count-a-1] = (num%10);
      num = num/10;
      if(num!=0)
        (input->digits_used)--;
      a++;
      if(num<10){
        data[*count-a-1] = num;
        (input->digits_used)--;
        break;
      }
    }
  }
  void copy(newera_hpc_data *src,newera_hpc_data *dst){
    re_init_data(dst);
    for(int temp_c1=src->digits_used;temp_c1<src->count;temp_c1++){
       dst->data[temp_c1] = src->data[temp_c1];
    }
    dst->digits_used = src->digits_used;
  }
  int check_zero(newera_hpc_data *n1){
    int *count = &(n1->count);
    if((n1->digits_used==(*count-1))&&n1->data[*count-1]==0)return 0;
    else return 1;
  }
};

