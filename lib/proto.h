#ifndef __PROTO_H__
#define __PROTO_H__

#include "address.h"

//Ether II Frame
#define ETH_TYPE_IP 0x0800
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_ROUTER 0x0801
#pragma pack(push, 1)
struct eth_header
{
    eth_addr h_dest; //目的MAC地址
    eth_addr h_source; //源MAC地址
    
    uint16_t h_proto ; //网络层所使用的协议类型
};
#pragma pack(pop)


/* Copy from include/net/arp.h 
 * ARP Protocol
*/
//! Ethernet hardware addresses.
#define ARPHRD_ETHER            1

/** \ingroup Network
 *  \defgroup NetARP ARP (Address Resolution Protocol) layer
 *  The ARP (Address Resolution Protocol) layer.
 *  @{
 */

//! ARP request operation.
#define ARP_OP_REQUEST          1
//! ARP reply operation.
#define ARP_OP_REPLY            2
//! RARP request operation.
#define RARP_OP_REQUEST         3
//! RARP reply operation.
#define RARP_OP_REPLY           4

// The ARP packet structure.;
#pragma pack(push, 1)
struct arp_t
{
        //! Format of hardware address.
        uint16_t arp_hard_type;
        //! Format of protocol address.
        uint16_t arp_proto_type;
        //! Length of hardware address.
        uint8_t  arp_hard_size;
        //! Length of protocol address.
        uint8_t  arp_proto_size;
        //! ARP operation code (command).
        uint16_t arp_op;
        //! Hardware source address.
        eth_addr arp_eth_source;
        //! IP source address.
        ip_addr arp_ip_source;
        //! Hardware destination address.
        eth_addr  arp_eth_dest;
        //! IP destination address.
        ip_addr arp_ip_dest;
};
#pragma pack(pop)

#pragma pack(push, 1) 
struct RTHeader{ 
    ip_addr ip;
    ip_addr ip_mask;
    eth_addr mac;
    uint8_t sz;
    uint8_t cnt;
};
#pragma pack(pop)
static size_t RTILen = 2 * sizeof(ip_addr);
#endif
