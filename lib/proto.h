#ifndef __PROTO_H__
#define __PROTO_H__

#include "address.h"

//Ether II Frame
#define ETH_TYPE_IP 0x0800
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_ROUTER 0xCCCB
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
struct arp_header
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
        uint16_t arp_opear;
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
    uint8_t sz;
	uint8_t type;
};
#pragma pack(pop)
#define RTILen (2 * sizeof(ip_addr))

#define IP_HDR_SIZE 20
#define DEFAULT_TTL 12
#define IP_PROTO_TCP 6
#pragma pack(push, 1) 
struct ip_header { 
	uint8_t hdr[2];
	uint16_t len; 
	uint16_t ident;
	uint16_t fragment_offset;
	uint8_t ttl;
	uint8_t proto;
	uint16_t checksum;
	ip_addr sour_addr;
	ip_addr dest_addr;
	ip_header(){
		memset(this, 0, IP_HDR_SIZE);
		hdr[0] = (4 << 4) + 5;
		hdr[1] = 0;
		ttl = DEFAULT_TTL; 
		proto = IP_PROTO_TCP;
	}
	void setDF(bool DF){
		if (DF)
			fragment_offset |= 1<<6;
		else fragment_offset &= ~(1<<6);
	}
	bool getDF(){
		return fragment_offset & (1<<6);
	}
	void setMF(bool MF){
		if (MF)
			fragment_offset |= 1<<5;
		else fragment_offset &= ~(1<<5);
	}
	bool getMF(){
		return fragment_offset & (1<<5);
	}
	void setFragment(uint16_t x) {
		fragment_offset = rev_16((rev_16(fragment_offset) >> 13) << 13);
		fragment_offset |= rev_16(x);
	}
	uint16_t getFragment() { 
		return rev_16(fragment_offset) & ((1<<13)-1);
	}
	uint8_t getIHL(){
		return 4*(hdr[0] & ((1<<4)-1));
	}
	void setIHL(uint8_t x) {
		x /= 4;
		hdr[0] = (((hdr[0] >> 4) << 4) | x);
	}
};
#pragma pack(pop)
#endif
