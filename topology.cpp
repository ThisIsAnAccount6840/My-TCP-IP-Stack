#include"graph.h"

using namespace std;

extern void network_start_pkt_receiver_thread(graph_t *);

graph_t* build_first_topo()
{
	//Create an empty graph
	graph_t* topo= create_new_graph("Hello World Generic Graph");
	//Create nodes in this graph
	node_t* R0_re=create_graph_node(topo,"R0_re");
	node_t* R1_re=create_graph_node(topo,"R1_re");
	node_t* R2_re=create_graph_node(topo,"R2_re");
	//Create links between two nodes
	insert_link_between_two_nodes(R0_re,R1_re,"eth0/0","eth0/1",1);
	insert_link_between_two_nodes(R0_re,R2_re,"eth0/4","eth0/5",1);
	insert_link_between_two_nodes(R1_re,R2_re,"eth0/2","eth0/3",1);

	//Add network properties
	node_set_ip_address(R0_re,"122.1.1.0");
	node_set_intf_ip_address(R0_re,"eth0/4","40.1.1.1",24);
	node_set_intf_ip_address(R0_re,"eth0/0","20.1.1.1",24);

        node_set_ip_address(R1_re,"122.1.1.1");
        node_set_intf_ip_address(R1_re,"eth0/1","20.1.1.2",24);
        node_set_intf_ip_address(R1_re,"eth0/2","30.1.1.1",24);
        
        node_set_ip_address(R2_re,"122.1.1.2");
        node_set_intf_ip_address(R2_re,"eth0/3","30.1.1.2",24);
        node_set_intf_ip_address(R2_re,"eth0/5","40.1.1.2",24);

		network_start_pkt_receiver_thread(topo);
	
	return topo;
}

graph_t *build_linear_topo(){
	graph_t* topo= create_new_graph("Linear Topology");
	
	node_t* H1=create_graph_node(topo,"H1");
	node_t* H2=create_graph_node(topo,"H2");
	node_t* H3=create_graph_node(topo,"H3");

	insert_link_between_two_nodes(H1, H2, "eth0/1", "eth0/2", 1);
	insert_link_between_two_nodes(H2, H3, "eth0/3", "eth0/4", 1);

	node_set_ip_address(H1, "122.1.1.1");
	node_set_ip_address(H2, "122.1.1.2");
	node_set_ip_address(H3, "122.1.1.3");

	node_set_intf_ip_address(H1, "eth0/1", "10.1.1.1", 24);
	node_set_intf_ip_address(H2, "eth0/2", "10.1.1.2", 24);
	node_set_intf_ip_address(H2, "eth0/3", "20.1.1.2", 24);
	node_set_intf_ip_address(H3, "eth0/4", "20.1.1.1", 24);

	network_start_pkt_receiver_thread(topo);

	return topo;
}