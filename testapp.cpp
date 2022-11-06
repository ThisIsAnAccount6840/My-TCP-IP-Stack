#include"graph.h"

extern graph_t* build_first_topo();

int main()
{
	graph_t* topo=build_first_topo();
	dump_graph(topo);
	dump_nw_graph(topo);
	return 0;
}
