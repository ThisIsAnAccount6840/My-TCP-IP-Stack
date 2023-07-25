//#include"graph.h"
#include"utils.h"
#include "comm.h"
#include "CommandParser/libcli.h"

extern graph_t* build_first_topo();
extern graph_t *build_linear_topo();
extern void nw_init_cli();

graph_t* topo=NULL;

int main(int argc, char* *argv){
	//nw_init_cli();

	/* PART A
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

	sleep(2);
	cout<<"===Testing Packet Transmission==="<<endl;
	node_t *sending_node=get_node_by_node_name(topo,"R0_re");
	interface_t *send_itf=get_node_if_by_name(sending_node, "eth0/0");
	char msg[]="hello world!";
	send_pkt_out(msg, sizeof(msg)/sizeof(char), send_itf); //经常失败

	*/
 
	//PART B
	topo=build_linear_topo();
	dump_graph(topo);
	dump_nw_graph(topo);

	//start_shell();
	return 0;
}
