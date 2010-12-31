#include <newerahpc/network.h>

using namespace newera_network;

int main(){
	start();
	hpc_data->load((char *)"grid_plugin/bin/libplugin.so");
	//hpc_data->load((char *)"grid_plugin/plugin.info");
	grid_task task;
	task.func_name = (char *)"newera_task";
	instruction_set *in1,*in2,*in3;
	in1 = new instruction_set;
	in2 = new instruction_set;
	in3 = new instruction_set;
	int *data1 = new int [3];
	int *data2 = new int [3];
	int *data3 = new int [3];
	data1[0] = 5;    ///number
	data1[1] = 10;   ///no. of loop
	data1[2] = 0;    ///initial count
	data2[0] = 239;  
	data2[1] = 5;
	data2[2] = 0;
	data3[0] = 239;
	data3[1] = 10;
	data3[2] = 5;
	in1->data = (void *)data1;
	in1->length = sizeof(int)*3;
	in2->data = (void *)data2;
	in2->length = sizeof(int)*3;
	in3->data = (void *)data3;
	in3->length = sizeof(int)*3;
	task = in1;
	task = in2;
	task = in3;
	cout<<"submitting task"<<endl;
	task.submit();
	cout<<"task givven"<<endl;
	task.check();
	cout<<"task collecting"<<endl;
	task.collect();
	sig_handler(SIGINT);
	return 0;
}
