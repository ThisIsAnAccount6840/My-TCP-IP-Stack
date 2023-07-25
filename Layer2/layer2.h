#ifndef __LAYER2__
#define __LAYER2__

#include "../net.h"
#include "../gluethread/glthread.h"

#define ETH_HDR_SIZE_EXCL_PAYLOAD (sizeof(ethernet_header_t)-sizeof(ethernet_header_t::payload))
#define ETH_CRC(eth_hdr_ptr, payload_size)   (*(unsigned int *)(((char *)(eth_hdr_ptr->payload) + payload_size))) //？
#define NODE_ARP_TABLE(node)    node->node_nw_prop.arp_table
#define GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr)  ethernet_hdr->payload

#pragma pack (push,1) //防止类各成员之间被padding，就不符合标准的header format了

class arp_header_t{
    public:
    // Constants
    short hw_type; //1 for ethernet table
    short proto_type; //0x0800 for IPv4
    char hw_addr_len; //6 for MAC
    char proto_addr_len; //4 for IPv4
    // Variables
    short op_code; //1-ARP broadcast request; 2-ARP reply
    mac_add_t  src_mac;
    unsigned int src_ip;
    mac_add_t dst_mac;
    unsigned int dst_ip;
};

class ethernet_header_t{
    public:
    mac_add_t src_mac_addr;
    mac_add_t dst_mac_addr;
    unsigned short type;//EtherType, i.e. length
    char payload[248];
    unsigned int CRC; //或Frame Check Sequence（FCS）, an error detecting code (循环冗余校验码)

};
#pragma pack (pop)

struct arp_entry_t
{
    ip_add_t ip_addr; //key
    mac_add_t mac_addr;
    char oif_name[IF_NAME_SIZE];//Outgoing interface, on which the arp reply message is received
    glthread_t arp_glue;
};
GLTHREAD_TO_STRUCT(arp_glue_to_arp_entry, arp_entry_t, arp_glue);

struct arp_table_t
{
    glthread_t arp_entries;
};

void init_arp_table(arp_table_t* *arp_table);

arp_entry_t * arp_table_lookup(arp_table_t *arp_table, char *ip_addr);

void clear_arp_table(arp_table_t *arp_table);

void delete_arp_table_entry(arp_table_t *arp_table, char *ip_addr);

bool arp_table_entry_add(arp_table_t *arp_table, arp_entry_t *arp_entry);

void dump_arp_table(arp_table_t *arp_table);

void arp_table_update_from_arp_reply(arp_table_t *arp_table, arp_header_t *arp_hdr, interface_t *iif);

void send_arp_broadcast_request(node_t *node, interface_t *oif, char *ip_addr);

void layer2_frame_recv(node_t *node, interface_t *interface, char *pkt, unsigned int pkt_size);

static inline ethernet_header_t* ALLOC_ETH_HDR_WITH_PAYLOAD(char *pkt, unsigned int pkt_size){ //似乎inline函数就可以定义在.h文件中
    ethernet_header_t *new_pkt=new ethernet_header_t();
    strncpy(new_pkt->payload, pkt, pkt_size);
    new_pkt->CRC=ETH_CRC(new_pkt, pkt_size);
    return new_pkt;
}

static inline bool l2_frame_recv_qualify_on_interface(interface_t *interface, ethernet_header_t *ethernet_hdr){
    if( ! IS_INTF_L3_MODE(interface))  return false;
    
    if(!strncmp(ethernet_hdr->dst_mac_addr.mac, IF_MAC(interface), 8))  return true;

    if(IS_MAC_BROADCAST_ADDR(ethernet_hdr->dst_mac_addr.mac)) return true;

    return false;
}

#endif