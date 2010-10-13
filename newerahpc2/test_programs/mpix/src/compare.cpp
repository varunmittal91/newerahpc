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
  int compare(newera_hpc_data *n1,newera_hpc_data *n2){
    if(n1->digits_used>n2->digits_used)return 1;
    else if(n1->digits_used<n2->digits_used)return -1;
    int *limit = &(n1->digits_used);
    int *count = &(n1->count);
    newera_hpc_bit *num1 = n1->data;
    newera_hpc_bit *num2 = n2->data;
    for(int temp_c1=*limit;temp_c1<*count;temp_c1++){
       if(num1[temp_c1]>num2[temp_c1])return -1;
       else if(num1[temp_c1]<num2[temp_c1])return 1;
    }
    return 0;
  }
};
