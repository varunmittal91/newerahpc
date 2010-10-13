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
#include <string>
#include <iostream>

using namespace std;

namespace newera_mpi{
   newera_hpc_data *arctan(newera_hpc_bit num,int start_d,int end_d){
       newera_hpc_data *power = init_data();
       newera_hpc_data *power_m = init_data();
       newera_hpc_data *power_t1 = init_data();
       newera_hpc_data *temp_data = init_data();
       newera_hpc_data *result = init_data();
       newera_hpc_bit power_count;
       int x1;
       int *count;
       int flag = 0;
       float quot;
       assign(power,1);
       for(int temp_c1=start_d;temp_c1<end_d;temp_c1++){
          cout<<temp_c1<<endl;
          power_count = 2*temp_c1+1;
          if(temp_c1==start_d){
             assign(power,num);
             if(temp_c1!=0){
                for(int temp_c2=0;temp_c2<start_d;temp_c2++){
                   multiply(power,num);
                   multiply(power,num);
                }
             }
          }
          else{
             multiply(power,num);
             multiply(power,num);
          }

          copy(power,power_m);
          multiply(power_m,power_count);
          copy(power_m,power_t1);
          
          x1 = power_m->digits_used;
          if(x1==0)break;
          re_init_data(power_m);
          power_m->data[--x1]=1;
          power_m->digits_used = x1;
          x1 = power_m->count-x1-1;
          temp_data->digits_used = x1;
          count = &(power->count);
          for(int temp_c2 = x1;temp_c2<*count;temp_c2++){
             if(check_zero(power_m)==0)break;
             devide(power_m,power_t1,&quot);
             multiply(power_m,10);
             temp_data->data[temp_c2] = quot;
          }
          if(flag==0){
             add(result,temp_data);
             flag=1;
          }
          else{
             subtract(result,temp_data);
             flag=0;
          }
          temp_data->digits_used = 0;
          re_init_data(temp_data);
          re_init_data(power_m);
       }
       delete_data(power);
       delete_data(power_m);
       delete_data(power_t1);
       delete_data(temp_data);
       return result;
   }
};
