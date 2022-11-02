#include"graph.h"
#include<string>
using namespace std;

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
	
	return topo;
}
