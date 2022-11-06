#include<memory.h>

//Forward declarations
class node_t;
class link_t;
class interface_t;
class graph_t;

class ip_add_t
{
	public:
		char ip_addr[16];//Format: A.B.C.D
};

class mac_add_t
{
	public:
		char mac[8];
};

class node_nw_prop_t
{
	public:
		bool is_ip_addr_config;// '1':IP address of this node is configured
		ip_add_t ip_addr;//IP address of this node
};

static inline void init_node_nw_prop(node_nw_prop_t *node_nw_prop)
{
	node_nw_prop->is_ip_addr_config=false;
	memset(node_nw_prop->ip_addr.ip_addr, 0, 16);
}

class intf_nw_prop_t
{
	public:
		mac_add_t mac_addr;
		bool is_ip_addr_config;
		ip_add_t ip_add;
		int mask;
};

static inline void init_intf_nw_prop(intf_nw_prop_t *i_p )
{
	memset(i_p->mac_addr.mac,0,8);
	i_p->is_ip_addr_config=false;
	memset(i_p->ip_add.ip_addr,0,16);
	i_p->mask=0;
}

void interface_assign_mac_address(interface_t *interface);


#define IF_MAC(intf_ptr)	(((intf_ptr)->intf_nw_prop).mac_addr.mac)
#define IF_IP(intf_ptr)        (((intf_ptr)->intf_nw_prop).ip_add.ip_addr)
#define NODE_IP_ADDR(node_ptr)        (((node_ptr)->node_nw_prop).ip_addr.ip_addr)
#define IS_INTF_L3_MODE(intf_ptr)	((intf_ptr)->intf_nw_prop.is_ip_addr_config)


// APIs to set network node properties
bool node_set_ip_address(node_t *node, const char *ip_addr);
bool node_set_intf_ip_address(node_t *node, const char *if_name, const char *ip_add, int mask);
bool node_unset_intf_ip_address(node_t *node, const char *if_name);

//Dumping Functions to dump network information on nodes and interfaces
void dump_nw_graph(graph_t *graph);
void dump_node_nw_prop(node_t *node);
void dump_intf_prop(interface_t *interface);

