#include "comm.h"
//#include "graph.h"
#include  <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <netdb.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>//close函数

using namespace std;

static unsigned int ini_udp_port_num=40000;
static char recv_buffer[MAX_PACKET_BUFFER_SIZE];
static char send_buffer[MAX_PACKET_BUFFER_SIZE];

unsigned int get_next_udp_port_num(){
    return ini_udp_port_num++;
} 

void init_udp_socket(node_t *node){
    node->udp_port_number=get_next_udp_port_num();
    int udp_socket_fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if(udp_socket_fd==-1) {
        cout <<"ERROR: Socket creation failed for node  "<<node->node_name<<endl;
        return;
    }

    //bind udp socket to IP address 127.0.0.1---???
    struct sockaddr_in node_addr;
    node_addr.sin_family      = AF_INET;
    node_addr.sin_port        = node->udp_port_number;
    node_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(udp_socket_fd, (struct sockaddr *)&node_addr, sizeof(struct sockaddr))==-1){
        cout<<"ERROR: Socket Binding failed for node "<<node->node_name<<endl;
        return;
    }
    node->udp_socket_fd=udp_socket_fd;//assign the socket file descriptor to the node
}

int pkt_receive(node_t *node, interface_t *interface, char *pkt, unsigned int pkt_size){
    // Entry point into data-link layer from physical layer
    cout<<"Packet Received: "<<pkt<<endl;
    cout<<"Receiving Node: "<<node->node_name<<" "<<"on Interface: "<<interface->if_name<<endl;
}

static void _pkt_receive(node_t *receving_node,  char *pkt_with_aux_data, unsigned int pkt_size){
    char *itf_name=pkt_with_aux_data;
    interface_t *recv_intf=get_node_if_by_name(receving_node, itf_name);
    if(!recv_intf) {
        cout<<"ERROR: No Such Interface Exists!"<<endl;
        return ;
    }
    pkt_receive(receving_node, recv_intf, pkt_with_aux_data+IF_NAME_SIZE, pkt_size-IF_NAME_SIZE);
}

void *  _network_start_pkt_receiver_thread(void *arg){
    node_t * node;
    glthread_t *curr;

    fd_set active_socket_fd_set, backup_socket_fd_set;
    FD_ZERO(&active_socket_fd_set);
    FD_ZERO(&backup_socket_fd_set);
    int socket_max_fd=0;
    int bytes_recvd=0;

    graph_t *topo=(graph_t*) arg;
    socklen_t addr_length(sizeof(struct sockaddr));

    struct sockaddr_in sender_addr;

    ITERATE_GLTHREAD_BEGIN(&(topo->node_list), curr){
        node=graph_glue_to_node(curr);
        if(!node->udp_socket_fd) continue;
        if(node->udp_socket_fd > socket_max_fd) socket_max_fd=node->udp_socket_fd;

        FD_SET(node->udp_socket_fd, &backup_socket_fd_set);

    }ITERATE_GLTHREAD_END(&(topo->node_list), curr)

    while(1){
        memcpy(&active_socket_fd_set, &backup_socket_fd_set, sizeof(fd_set));
        select(socket_max_fd + 1, &active_socket_fd_set, NULL, NULL, NULL);//only when one of the sockets receives data will this process becomes unblocked

        ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){
            node=graph_glue_to_node(curr);
            if(FD_ISSET(node->udp_socket_fd, &active_socket_fd_set)){
                memset(recv_buffer, 0, MAX_PACKET_BUFFER_SIZE);
                bytes_recvd = recvfrom(node->udp_socket_fd, (char *)recv_buffer, 
                        MAX_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&sender_addr, &addr_length);
                
                _pkt_receive(node, recv_buffer, bytes_recvd);
            }

        }ITERATE_GLTHREAD_END(&topo->node_list, curr)
    }
}

void network_start_pkt_receiver_thread(graph_t *topo){
    //start a thread to listen to all udp ports
    pthread_attr_t attr;
    pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&recv_pkt_thread, &attr, _network_start_pkt_receiver_thread,  (void *)topo);
}

static int _send_pkt_out(int sock_fd, char *pkt_data, unsigned int pkt_size, unsigned int dst_udp_port_no){
    int rc;
    struct sockaddr_in dest_addr;
   
    struct hostent *host = (struct hostent *) gethostbyname("127.0.0.1"); 
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = dst_udp_port_no;
    dest_addr.sin_addr = *((struct in_addr *)host->h_addr);

    rc = sendto(sock_fd, pkt_data, pkt_size, 0, 
            (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));// send data to operating system
    
    return rc;
}

int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface){
    int rc=0;
    node_t *send_node=interface->att_node;
    node_t *recev_node=get_nbr_node(interface);
    if(!recev_node) return -1;

    unsigned int recev_udp_port_num=recev_node->udp_port_number;
    int sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock<0) {
        cout<<"ERROR: Socket Creation Failure"<<endl;
        return -1;
    }
    interface_t *other_intf=&(interface->link->intf1)==interface?&(interface->link->intf2):&(interface->link->intf1);

    memset(send_buffer, 0, MAX_PACKET_BUFFER_SIZE);
    char *pkt_with_aux_data=send_buffer;
    strncpy(pkt_with_aux_data, other_intf->if_name, IF_NAME_SIZE);
    pkt_with_aux_data[IF_NAME_SIZE]='\0';
    memcpy(pkt_with_aux_data+IF_NAME_SIZE, pkt, pkt_size);

    rc = _send_pkt_out(sock, pkt_with_aux_data, pkt_size + IF_NAME_SIZE,  recev_udp_port_num);
    
    close(sock);

    return rc;
}

void send_pkt_flood(node_t *node, interface_t *exempted_itf, char *pkt, unsigned int pkt_size){
    int i;
    for(i=0;i<MAX_INTF_PER_NODE;i++){
        if(node->intf[i]==NULL) return;
        if(node->intf[i] != exempted_itf) send_pkt_out(pkt, pkt_size, node->intf[i]);
    }
    return ;
}