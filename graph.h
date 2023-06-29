#include "gluethread/glthread.h"
#include"net.h"
#include<cassert>
#include<cstring>
#include<cstddef>

//define constants
#define NODE_NAME_SIZE 16
#define IF_NAME_SIZE 16
#define MAX_INTF_PER_NODE 10

//forward declaration
class node_t;
class link_t;

class interface_t {

	public:
	 	char if_name[IF_NAME_SIZE]; //Name of the interface
		node_t* att_node; //The node that the interface is attached to
		link_t* link; //The link of the interface
		intf_nw_prop_t intf_nw_prop;//The network property of an interface--encapsuled in a class.
};

class link_t{

	public:
		interface_t intf1;
		interface_t intf2;
		unsigned int cost;
};

class node_t{

	public:
		char node_name[NODE_NAME_SIZE];
		interface_t* intf[MAX_INTF_PER_NODE];//All the slots that a node has. When an element is null in this array, it means that no interface is plugged into this slot.
		glthread_t graph_glue; 
		node_nw_prop_t node_nw_prop;//The network property of a node--encapsuled in a class.
};
GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue);//?


class graph_t{

	public:
		char topology_name[32];
		glthread_t node_list; //a special form of link list
};

static inline node_t* get_nbr_node(interface_t* interface)
{
	//if this interface is non-exist or hasn't linked to another node
	assert(interface->att_node);
	assert(interface->link);

	link_t* link=interface->link;
	interface_t *itf1,*itf2,*nbr_itf;
	itf1=&link->intf1;
	itf2=&link->intf2;

	if(itf1==interface) nbr_itf=itf2;
	else nbr_itf=itf1;

 	return nbr_itf->att_node;
}

static inline int get_node_available_slot(node_t* node)
{
	int i;
	for(i=0;i<MAX_INTF_PER_NODE;i++)
	{
		if(node->intf[i]==NULL) return i;
	}
	return -1;
}

static inline interface_t* get_node_if_by_name(node_t* node, const char* if_name)
{
	int i;
	for(i=0;i<MAX_INTF_PER_NODE && node->intf[i];i++)
	{
		if(!strncmp((node->intf[i])->if_name,if_name,IF_NAME_SIZE)) return node->intf[i];
	}
	return NULL;
}

static inline node_t* get_node_by_node_name(graph_t* topo, const char* node_name)
{
	node_t* node;
	glthread_t* curr;

	ITERATE_GLTHREAD_BEGIN(&topo->node_list,curr)
	{
		node=graph_glue_to_node(curr);
		if(strncmp(node->node_name,node_name,strlen(node_name))==0) return node;
	} ITERATE_GLTHREAD_END(&topo->node_list,curr);
	return NULL;
}

//Function declarations
graph_t* create_new_graph(const char * topology_name);
node_t* create_graph_node(graph_t* graph, const char* node_name);
void insert_link_between_two_nodes
(node_t* node1, node_t* node2, const char* from_if_name, const char* to_if_name, unsigned int cost);

/*Display Routines*/
void dump_graph(graph_t *graph);
void dump_node(node_t *node);
void dump_interface(interface_t *interface);
