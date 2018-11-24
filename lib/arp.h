#ifndef __ARP_H__
#define __ARP_H__
#include<map>
#include<lock>
#include "address.h"
class ArpManager{
    std::map<ip_addr, std::lock> ArpRequest；
    std::map<ip_addr, eth_addr> ArpMapping;
    void ReceiveFunc();
    std::Thread Receiver();
    void sendArp();
    pubilc:
    ArpManager();
    ~ArpManager();
    getMacAddr(ip_addr);
}

//! Ethernet hardware addresses.
/* Copy from include/net/arp.h 
 * ARP Protocol
*/
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
#endif