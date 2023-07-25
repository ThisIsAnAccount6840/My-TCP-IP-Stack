#include "../graph.h"
#include "layer2.h"
#include "../tcpconst.h"
#include "../comm.h"
#include<iostream>

using namespace std;

void L2_frame_recv(node_t * node, interface_t * interface, char * pkt, unsigned int pkt_size){

}

void init_arp_table(arp_table_t* *arp_table){
    *arp_table=new arp_table_t();
    init_glthread(&((*arp_table)->arp_entries));
}

arp_entry_t * arp_table_lookup(arp_table_t *arp_table, char *ip_addr){
    glthread_t *curr;
    arp_entry_t *arp_entry;
    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr){
        arp_entry=arp_glue_to_arp_entry(curr);
        if(!strncmp(arp_entry->ip_addr.ip_addr, ip_addr, 16)) return arp_entry;
    }ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr)
    return NULL;
}

void clear_arp_table(arp_table_t *arp_table);

void delete_arp_table_entry(arp_table_t *arp_table, char *ip_addr){
    arp_entry_t *arp_entry=arp_table_lookup(arp_table, ip_addr);
    if(arp_entry==NULL) return;// Entry not found

    remove_glthread(&arp_entry->arp_glue); //Remove node from the linked-list
    delete arp_entry;
}

bool arp_table_entry_add(arp_table_t *arp_table, arp_entry_t *arp_entry){
    arp_entry_t *ap=arp_table_lookup(arp_table, arp_entry->ip_addr.ip_addr);
    if(ap){
        if(!strncmp(ap->mac_addr.mac, arp_entry->mac_addr.mac, 8)  && !strncmp(ap->oif_name, arp_entry->oif_name, 16)) return false;

        delete_arp_table_entry(arp_table, ap->ip_addr.ip_addr);//Delete the old entry
    }
    init_glthread(&arp_entry->arp_glue);
    glthread_add_next(&arp_table->arp_entries, &arp_entry->arp_glue);
    return true;
}

void dump_arp_table(arp_table_t *arp_table){
    glthread_t *curr;
    arp_entry_t *entry;
    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr){
        entry=arp_glue_to_arp_entry(curr);
        cout<<"IP Address:  "<<entry->ip_addr.ip_addr<<endl;
        cout<<"MAC Address:  ";
        for(int i=0; i<7; i++) cout<<entry->mac_addr.mac[i]<<":";
        cout<<entry->mac_addr.mac[7]<<endl;
        cout<<"OIF:  "<<entry->oif_name<<endl;
    }ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr)
}

void arp_table_update_from_arp_reply(arp_table_t *arp_table, arp_header_t *arp_hdr, interface_t *iif){
    if(arp_hdr->op_code != ARP_REPLY){
        cout<<"ERROR: Not ARP REPLY"<<endl;
        return;
    }
    arp_entry_t *new_entry=new arp_entry_t();
    convert_ip_from_int_to_str(arp_hdr->src_ip, new_entry->ip_addr.ip_addr);
    memcpy(new_entry->mac_addr.mac, arp_hdr->src_mac.mac, sizeof(mac_add_t));
    strncpy(new_entry->oif_name, iif->if_name, IF_NAME_SIZE);

    bool res=arp_table_entry_add(arp_table, new_entry);
    if(res==false) delete new_entry;
}

void send_arp_broadcast_request(node_t *node, interface_t *oif, char *ip_addr){
    ethernet_header_t *ethernet_hdr = (ethernet_header_t*)calloc(1, ETH_HDR_SIZE_EXCL_PAYLOAD +  sizeof(arp_header_t));//手动分配空间,因为payload空间是变化的
    if(!oif){
        oif=node_get_matching_subnet_interface(node, ip_addr);
        if(!oif){
            cout<<"ERROR: ARP Resolution Impossible"<<endl;
        }
        //Step1: Prepare Ethernet Header
        layer2_fill_with_broadcast_mac(ethernet_hdr->dst_mac_addr.mac);
        memcpy(ethernet_hdr->src_mac_addr.mac, IF_MAC(oif), sizeof(mac_add_t));
        ethernet_hdr->type=ARP_MSG;

        //Step2: Prepare ARP Broadcast Request
        arp_header_t *arp_hdr=(arp_header_t*) ethernet_hdr->payload;
        arp_hdr->hw_type=1;
        arp_hdr->proto_type=0x0800;
        arp_hdr->hw_addr_len=sizeof(mac_add_t);
        arp_hdr->proto_addr_len=4;

        arp_hdr->op_code=ARP_BROAD_REQ;
        memcpy(arp_hdr->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));
        inet_pton(AF_INET, IF_IP(oif), &arp_hdr->src_ip);
        arp_hdr->src_ip=htonl(arp_hdr->src_ip);
        memset(arp_hdr->dst_mac.mac, 0, sizeof(mac_add_t));
        inet_pton(AF_INET, ip_addr, &arp_hdr->dst_ip);
        arp_hdr->dst_ip=htonl(arp_hdr->dst_ip);
        ETH_CRC(ethernet_hdr, sizeof(arp_header_t))=0; 

        //Step3: Send out the ARP Broadcast Request
        send_pkt_out((char*)ethernet_hdr, ETH_HDR_SIZE_EXCL_PAYLOAD +  sizeof(arp_header_t), oif);
        free(ethernet_hdr);
    }
}

static void send_arp_reply_msg(ethernet_header_t * ethernet_hdr_in, interface_t *oif){
    arp_header_t *arp_hdr_in=(arp_header_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr_in));
    ethernet_header_t *ethernet_hdr_reply=(ethernet_header_t *)calloc(1, MAX_PACKET_BUFFER_SIZE);

    memcpy(ethernet_hdr_reply->dst_mac_addr.mac, arp_hdr_in->src_mac.mac, sizeof(mac_add_t));
    memcpy(ethernet_hdr_reply->src_mac_addr.mac, IF_MAC(oif),sizeof(mac_add_t));

    ethernet_hdr_reply->type=ARP_MSG;
    arp_header_t *arp_hdr_reply=(arp_header_t *)GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr_reply);

    arp_hdr_reply->hw_type=1;
    arp_hdr_reply->proto_type=0x0800;
    arp_hdr_reply->hw_addr_len=sizeof(mac_add_t);
    arp_hdr_reply->proto_addr_len=4;

    arp_hdr_reply->op_code=ARP_REPLY;
    memcpy(arp_hdr_reply->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));
    inet_pton(AF_INET, IF_IP(oif), &arp_hdr_reply->src_ip);
    arp_hdr_reply->src_ip=htonl(arp_hdr_reply->src_ip);
    memcpy(arp_hdr_reply->dst_mac.mac, arp_hdr_in->src_mac.mac, sizeof(mac_add_t));
    arp_hdr_reply->dst_ip=arp_hdr_in->src_ip;
    ETH_CRC(ethernet_hdr_reply, sizeof(arp_header_t))=0;
    
    unsigned int total_packet_size=ETH_HDR_SIZE_EXCL_PAYLOAD+sizeof(arp_header_t);

    char *shifted_pkt_buffer=pkt_buffer_shift_right((char*) ethernet_hdr_reply, total_packet_size, MAX_PACKET_BUFFER_SIZE);

    send_pkt_out(shifted_pkt_buffer, total_packet_size, oif);
    free(ethernet_hdr_reply);
}

static void process_arp_reply_msg(node_t *node, interface_t *iif, ethernet_header_t *ethernet_hdr){
    cout<<"ARP Reply Msg Received on the interface "<<iif->if_name<<" of node "<<iif->att_node->node_name<<endl;
    //Insert an ARP entry to the ARP table
    arp_table_update_from_arp_reply(NODE_ARP_TABLE(node), (arp_header_t *)(GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr)), iif);
}

static void process_arp_broadcast_request(node_t *node, interface_t *iif, ethernet_header_t *ethernet_hdr){
    cout<<"ARP Broadcast Request Received on node "<<iif->att_node->node_name<<" on interface "<<iif->if_name<<endl;
    char ip_add[16];
    arp_header_t * arp_hdr=(arp_header_t *) (GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr));
    unsigned int arp_dst_ip=htonl(arp_hdr->dst_ip);
    inet_ntop(AF_INET, &arp_dst_ip, ip_add, 16);//转换成点分十进制
    ip_add[15]='\0';

    if(strncmp(ip_add, IF_IP(iif), 16)){
        cout<<"ARP Broadcast Rquest Msg Dropped, DST IP Does Not Match with the Interface IP: "<<IF_IP(iif)<<" of Node "<<node->node_name<<endl;
        return ;
    }
    send_arp_reply_msg(ethernet_hdr, iif);
}

void layer2_frame_recv(node_t *node, interface_t *interface, char *pkt, unsigned int pkt_size){
    ethernet_header_t *ethernet_hdr=(ethernet_header_t*)pkt;

    if(l2_frame_recv_qualify_on_interface(interface, ethernet_hdr)==false){
        cout<<"L2  Frame Rejected"<<endl;
        return;
    }

    cout<<"L2 Frame Accepted"<<endl;
    switch(ethernet_hdr->type){
        case ARP_MSG:
        {
            arp_header_t *arp_hdr=(arp_header_t*)(ethernet_hdr->payload);
            switch(arp_hdr->op_code){
                case ARP_BROAD_REQ:
                {
                    process_arp_broadcast_request(node, interface, ethernet_hdr);
                    break;
                }
                case ARP_REPLY:
                {
                    process_arp_reply_msg(node, interface, ethernet_hdr);
                    break;
                }
                default:
                break;
            }
            break;
        }
        default:
            //promote_pkt_to_layer3(node, interface, pkt, pkt_size);
            break;
    }
}