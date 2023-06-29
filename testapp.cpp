#include"graph.h"
#include"utils.h"
#include "CommandParser/libcli.h"

extern graph_t* build_first_topo();
extern void nw_init_cli();

graph_t* topo=NULL;

int main(int argc, char* *argv){
	nw_init_cli();
	topo=build_first_topo();
	dump_graph(topo);
	dump_nw_graph(topo);
	char str_prefix[16];
	char ip[16];
	char mac_arr[6];
	apply_mask("122.1.1.2",24,str_prefix);
	
	//something wrong with this function and the macro
	layer2_fill_with_broadcast_mac(mac_arr);
	cout<<"Testing function 'layer2_fill_with_broadcast_mac': "<<mac_arr[0]<<endl;
	cout<<"Testing macro: "<<IS_MAC_BROADCAST_ADDR(mac_arr)<<endl;
	
	cout<<"Testing convert ip->integer: "<<convert_ip_from_str_to_int(str_prefix)<<endl;
	convert_ip_from_int_to_str(2046886144,ip);
	cout<<"Testing convert int->ip: "<<ip<<endl;
	cout<<"Testing function 'apply_mask: '"<<str_prefix<<endl;

	start_shell();
	return 0;
}
