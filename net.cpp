#include"graph.h"
#include"utils.h"
#include<cstring>
#include<memory.h>
#include<iostream>
using namespace std;

bool node_set_ip_address(node_t *node, const char *ip_addr)
{
	assert(ip_addr);
	/*
	node->node_nw_prop.is_ip_addr_config=true;
	strncpy(node->node_nw_prop.ip_addr.ip_addr,in_addr,16);
	node->node_nw_prop.ip_addr.ip_addr[15]='\0';*/
	//Use macros
	node->node_nw_prop.is_ip_addr_config=true;
        strncpy(NODE_IP_ADDR(node),ip_addr,16);
        NODE_IP_ADDR(node)[15]='\0';

       return true;
}

bool node_set_intf_ip_address(node_t *node, const char *if_name, const char *ip_add, int mask)
{
	assert(if_name);
	assert(ip_add);

	interface_t *intf=get_node_if_by_name(node,if_name);
	if(!intf) assert(0);

	intf->intf_nw_prop.is_ip_addr_config=true;
	strncpy(IF_IP(intf),ip_add,16);
	IF_IP(intf)[15]='\0';
	intf->intf_nw_prop.mask=mask;
	
	return true;
}

bool node_unset_intf_ip_address(node_t *node, const char *if_name)
{
	assert(if_name);

        interface_t *intf=get_node_if_by_name(node,if_name);
        //if(!intf) assert(0);
		assert(intf);

        intf->intf_nw_prop.is_ip_addr_config=false;
        memset(IF_IP(intf),0,16);
	
	return true;

}

interface_t * node_get_matching_subnet_interface(node_t*node,const char*ip_addr)
{
	int mask,i;
	char intf_subnet[16], pass_subnet[16];
	
	for(i=0;i<MAX_INTF_PER_NODE && node->intf[i];i++)
	{
		mask=node->intf[i]->intf_nw_prop.mask;
		apply_mask(ip_addr,mask,pass_subnet);
		apply_mask(IF_IP(node->intf[i]),mask,intf_subnet);
		if(strcmp(pass_subnet,intf_subnet)==0) return node->intf[i];
	}
	return NULL;

}

unsigned int convert_ip_from_str_to_int(const char*ip_addr)
{
	unsigned int ip_int=0;
	// "inet_pton": transfer ip address from A.B.C.D->binary unsigned integer
	inet_pton(AF_INET,ip_addr,&ip_int);
	return htonl(ip_int);
}

void convert_ip_from_int_to_str(unsigned int ip_int, char* output_buffer)
{
	ip_int=ntohl(ip_int);
	inet_ntop(AF_INET,&ip_int,output_buffer,16);
	output_buffer[15]='\0';
}
/*interface_t * node_get_matching_subnet_interface(node_t *node, char *ip_addr)
{
	int i=0,j,max;
	for(;node->intf[i];i++)
	{
		mask=(node->intf[i])->intf_nw_prop.mask;
		for(j=0,max=mask;j<max;j++)
		{
			if(IF_IP(node->intf[i])[j]=='.') max++;
			if(IF_IP(node->intf[i])[j]!=ip_addr[j]) break;
		}
		if(j==max) return node->intf[i];
	}
	return NULL;
}



unsigned int convert_ip_from_str_to_int(char *ip_addr)
{
	char ip_add_int[33];
	ip_add_int[32]='\0';
	int i,j;
	unsigned int ip_int;
	for(i=0,j=0;i<16;)
	{
		if(ip_addr[i]!='.') 
		{
			ip_add_int[j]=ip_addr[i];
			i++;
			j++;
		}
		else i++;
	}

	ip_int=stoi(ip_add_int);
	return ip_int;
}

void convert_ip_from_int_to_str(unsigned int ip_addr, char *output_buffer);
*/

//Random number generator
static unsigned int hash_code(void *ptr, unsigned int size)
{
	unsigned int value=0,i=0;
	char *str=(char*) ptr;
	while(i<size)
	{
		value+=*str;
		value*=97;
		str++;
		i++;
	}
	return value;
}

void interface_assign_mac_address(interface_t *interface)
{
	node_t *node=interface->att_node;
	if(!node) return;

	unsigned int hash_value=0;
	hash_value=hash_code(node->node_name,NODE_NAME_SIZE);
	hash_value*=hash_code(interface->if_name,IF_NAME_SIZE);
	memset(IF_MAC(interface),0,sizeof(IF_MAC(interface)));
	memcpy(IF_MAC(interface),(char*) &hash_value,sizeof(unsigned int));//?

}

void dump_nw_graph(graph_t *graph)
{	
	node_t *node;
	glthread_t *curr;
	interface_t *interface;
	unsigned int i;
	cout<<"Topology Name: "<<graph->topology_name<<endl;
    	ITERATE_GLTHREAD_BEGIN(&graph->node_list, curr){

        node = graph_glue_to_node(curr);
        dump_node_nw_prop(node);
        for( i = 0; i < MAX_INTF_PER_NODE; i++){
            interface = node->intf[i];
            if(!interface) break;
            dump_intf_prop(interface);
        }
    } ITERATE_GLTHREAD_END(&graph->node_list, curr);
}

void dump_node_nw_prop(node_t *node)
{
	cout<<"Node Name: "<<node->node_name<<endl;
	if(node->node_nw_prop.is_ip_addr_config) 
		cout<<'\t'<<"IP Address: "<<NODE_IP_ADDR(node)<<endl;
}

void dump_intf_prop(interface_t *interface)
{
	dump_interface(interface);

	if(interface->intf_nw_prop.is_ip_addr_config)
		cout<<'\t'<<"IP Address: "<<IF_IP(interface)<<"/"<<interface->intf_nw_prop.mask<<endl;
	else 
		cout<<'\t'<<"IP Address: "<<"Nil/0"<<endl;

	cout<<'\t'<<"MAC Address: "<<IF_MAC(interface)[0]<<":"<<IF_MAC(interface)[1]<<":"<<IF_MAC(interface)[2]<<":"<<IF_MAC(interface)[3]<<":"<<IF_MAC(interface)[4]<<":"<<IF_MAC(interface)[5]<<endl;
}

char * pkt_buffer_shift_right(char *pkt, unsigned int pkt_size, unsigned int total_buffer_size){

}