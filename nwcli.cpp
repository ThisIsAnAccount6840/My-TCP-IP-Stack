#include "CommandParser/libcli.h"
#include  "CommandParser/cmdtlv.h"
#include  "cmdcodes.h"
#include "graph.h"
#include<iostream>

using namespace std;

extern graph_t *topo;

int  node_name_val(char * name){
    node_t *node;
    glthread_t *curr;
    
    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){
        node=graph_glue_to_node(curr);
        if(strcmp(name, node->node_name) == 0) return VALIDATION_SUCCESS;
    }ITERATE_GLTHREAD_END(&topo->node_list, curr);
    cout<<"ERROR! INVALID NODE NAME"<<endl;
    return VALIDATION_FAILED;
}

int  ip_add_val(char * ip){

}

static int arp_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable){
    tlv_struct_t *tlv = NULL;
    char *node_name=NULL;
    char node_name_id[]="node-name";
    char ip_add_id[]="ip-address";
    char *ip_add=NULL;
    TLV_LOOP_BEGIN(tlv_buf, tlv){
        if(strcmp(tlv->leaf_id, node_name_id) == 0) node_name=tlv->value;
        if(strcmp(tlv->leaf_id, ip_add_id) == 0) ip_add=tlv->value;
    }TLV_LOOP_END(tlv_buf, tlv);
    cout<<"Node name: "<<*node_name<<'/t'<<"IP Address: "<<*ip_add<<endl;
    return 0;
}

static int show_nw_topology_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enable_or_disable){
    int CMD_CODE=-1;
    CMD_CODE=EXTRACT_CMD_CODE(tlv_buf);

    switch(CMD_CODE){
        case CMDCODE_SHOW_NW_TOPO:
            dump_nw_graph(topo);
            break;
        default:
            break;
    }
}

void nw_init_cli(){
    init_libcli();

// Import all the hooks
    param_t *show = libcli_get_show_hook();
    param_t *debug = libcli_get_debug_hook();
    param_t *config = libcli_get_config_hook();
    param_t *debug_show = libcli_get_debug_show_hook();
    param_t *root = libcli_get_root();
    param_t *run = libcli_get_run_hook();

    {
        // Command: show topology
        static param_t topology;
        char cmd_name[]="topology";
        char help_des[]="Dump the complete network topology.";
        init_param(&topology, CMD, cmd_name, show_nw_topology_handler, 0, INVALID, 0,help_des);
        libcli_register_param(show, &topology);
        set_param_cmd_code(&topology, CMDCODE_SHOW_NW_TOPO);
    }
    {
        //Command: run node <node-name> resolve-arp <ip-address>
        param_t node;
        char node_name[]="node";
        char node_help[]="Input /'node/'";
        init_param(&node, CMD, node_name, NULL, NULL, INVALID, 0, node_help);
        libcli_register_param(run, &node);
        {
            param_t node_name;
            char node_name_help[]="Input the node name.";
            char node_name_id[]="node-name";
            init_param(&node_name, LEAF, NULL, NULL, node_name_val, STRING, node_name_id, node_name_help);
            libcli_register_param(&node, &node_name);
            {
                param_t res_arp;
                char res_arp_name[]="resolve-arp";
                char res_arp_help[]="Input /' resolve-arp/'";
                init_param(&res_arp, CMD, res_arp_name, NULL, NULL, INVALID, NULL, res_arp_help);
                libcli_register_param(&node_name, &res_arp);
                {
                    param_t ip_add;
                    char ip_add_help[]="Input the ip address in the A.B.C.D. form.";
                    char ip_add_leaf_id[]="ip-address";
                    init_param(&ip_add, LEAF, NULL, arp_handler, ip_add_val, STRING, ip_add_leaf_id, ip_add_help);
                    libcli_register_param(&res_arp, &ip_add);
                    set_param_cmd_code(&ip_add, ARP_RES_CMD_CODE);
                }
            }
        }
    }
    support_cmd_negation(config);

}