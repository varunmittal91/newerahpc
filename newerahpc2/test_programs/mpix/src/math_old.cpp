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

namespace newera_mpi{
   void add_old(newera_hpc_bit *resultx,newera_hpc_bit *temp_datax,int count){
      for(int temp_c3=(count-1);temp_c3>0;temp_c3--){
          resultx[temp_c3]=resultx[temp_c3]+temp_datax[temp_c3];
          if(resultx[temp_c3]>9){
             resultx[temp_c3]=resultx[temp_c3]%10;
             resultx[temp_c3-1] = resultx[temp_c3-1]+1;
          }
      }
      resultx[0]+=temp_datax[0];
   }
   void subtract_old(newera_hpc_bit *num1,newera_hpc_bit *num2,int count){
   for(int temp_c3=(count-1);temp_c3>0;temp_c3--){
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
   }
   void multiply_old(newera_hpc_bit *result,int num,int count){
    newera_hpc_bit temp_q=0,temp_r=0;
     for(int a=(count-1);a>=0;a--){
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
   }
};
