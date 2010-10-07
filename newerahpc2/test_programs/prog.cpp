#include <newerahpc/network.h>
#include <pthread.h>

using namespace std;
using namespace newera_network;

int main(){
	start();
	hpc_data->load((char *)"grid_plugin/bin/libplugin.so");
	grid_task task1;
	task1.func_name = (char *)"newera_task";
	instruction_set *in1 = new instruction_set;
	int *data = new int;
	*data = 12;
	in1->length = sizeof(int);
	in1->data = (void *)data;
	task1 = in1;
	task1.submit();
	task1.wait();
	return 0;
}
