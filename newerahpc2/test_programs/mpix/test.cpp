#include "include/init.h"
#include <iostream>

using namespace std;
using namespace newera_mpi;

void print_num(newera_hpc_bit *num,int count){
        cout<<num[0]<<" "<<endl;
        int b =0;
        for(int a=1;a<count;a++){
                cout<<num[a];
                if(a%10==0){
                        cout<<" ";
                        b++;
                        if(b==5){
                                cout<<endl;
                                b=0;
                        }
                }
        }
        cout<<endl;
}


int main(){
	int a=0,b=50;
        newera_hpc_data *result5   = arctan(5,a,b);
        newera_hpc_data *result239 = arctan(239,a,b);
        multiply_old(result5->data,16,result5->count);
        multiply_old(result239->data,4,result239->count);
        subtract_old(result5->data,result239->data,result5->count);
	print_num(result5->data,result5->count);
	return 0;
}
