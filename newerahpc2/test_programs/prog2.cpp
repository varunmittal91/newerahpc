#include <newerahpc/network.h>

using namespace newera_network;

int main(){
	start();
        hpc_data->load((char *)"grid_plugin/bin/libplugin.so");
	int count = 1;
	grid_task task;
	task.func_name = "newera_task";
	instruction_set *in1,*in2;
	in1 = new instruction_set;
	in2 = new instruction_set;
	int *data1 = new int [2];
	int *data2 = new int [2];
	data1[0] = 5;
	data1[1] = 10;
	data2[0] = 239;
	data2[1] = 10;
	in1->data = (void *)data1;
	in1->length = sizeof(int)*2;
	in2->data = (void *)data2;
	in2->length = sizeof(int)*2;
	cout<<"sommethings working"<<endl;
	task = in1;
	task = in2;
	cout<<"all done"<<endl;
	task.submit();
	task.check();
	cout<<"task submitted"<<endl;
	task.collect();
	return 0;
}
