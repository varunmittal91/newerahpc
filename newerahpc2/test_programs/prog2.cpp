#include <newerahpc/network.h>

using namespace newera_network;

int main(){
	start();
        hpc_data->load((char *)"grid_plugin/bin/libplugin.so");
	grid_task *task[10];
	instruction_set *in[10];
	for(int a=0;a<3;a++){
		task[a] = new grid_task;
		in[a] = new instruction_set;
		task[a]->func_name = (char *)"newera_task";
		(*task[a]) = in[a];
		int *data = new int;
		*data = 12;
		in[a]->length = sizeof(int);
		in[a]->data   = (void *)data;
		(*task[a]).submit();
		cout<<"task accepted"<<endl;
	}
	for(int a=0;a<3;a++){
		(*task[a]).collect();
	}
	while(1);
}
