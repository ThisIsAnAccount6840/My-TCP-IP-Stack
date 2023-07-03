#include "graph.h"

#define MAX_PACKET_BUFFER_SIZE 2048

int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);//pkt指向存储有发送数据包的buffer，interface指向发包的接口

int pkt_receive(node_t *node, interface_t *interface, char *pkt, unsigned int pkt_size);//数据包在这个函数里面从物理层进入了数据链路层

void send_pkt_flood(node_t *node, interface_t *exempted_itf, char *pkt, unsigned int pkt_size);//从除了exempted interface之外的其他所有interface发送数据包