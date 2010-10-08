#include <newerahpc/network.h>

using namespace newera_network;

int main(){
	start();
        hpc_data->load((char *)"grid_plugin/bin/libplugin.so");
	int count = 10;
	grid_task *task[count];
	instruction_set *in[count];
	for(int a=0;a<count;a++){
		task[a] = new grid_task;
		in[a] = new instruction_set;
		task[a]->func_name = (char *)"newera_task";
		(*task[a]) = in[a];
		int *data = new int;
		*data = a;
		in[a]->length = sizeof(int);
		in[a]->data   = (void *)data;
		(*task[a]).submit();
		cout<<"task accepted"<<endl;
	}
	for(int a=0;a<count;a++){
		(*task[a]).collect();
	}
	return 0;
}
