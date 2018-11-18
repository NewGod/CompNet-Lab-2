#include "device.h"
#include "packetio.h"
#include "ip.h"
#include "protocol.h"

int get_mac_address(const struct in_addr ip, struct ether_addr *mac) {
	for (struct Destination* i = dest_list; i; i = i->nxt) {
		if (i->ip.s_addr == ip.s_addr) {
			mac = &i->mac;
			return 0;
		}
	}
	arp_t arp;
	arp.arp_hard_type = ARPHRD_ETHER;
	arp.arp_proto_type = 0x0800;
	arp.arp_hard_size = ETH_ALEN;
	arp.arp_proto_size = 4;
	arp.arp_op = ARP_OP_REQUEST;
	int id = 0;
	Dev *device = getDevice(id);
	memcpy(arp.arp_eth_source, &device->mac, sizeof(struct ether_addr));
	arp.arp_ip_source = device->ip;
	memcpy(arp.arp_eth_source, &broadcast_addr.mac, sizeof(struct ether_addr));
	arp.arp_ip_dest = ip.s_addr;
	sendFrame(&arp, sizeof(arp), ETH_TYPE_ARP, arp.arp_eth_source, -1);
	char a[MAX_PCAP_FILTER_LEN]; 
	char src[MAX_IPV4_LEN], dst[MAX_MAC_LEN];
	ether2str(arp.arp_eth_source, dst);
	inet_pton(AF_INET,src,arp.arp_ip_dest);
	sprintf(a, "arp src %s and arp dst %s", src, dst);
	if (getFrame(a, &arp)) {
		return -1;
	}
	Destination* x = malloc(sizeof(Destination));
	memcpy(&x->ip, &arp.arp_ip_source, sizeof(x->ip));
	memcpy(&x->mac, arp.arp_eth_source, sizeof(x->mac));
	x->nxt = dest_list;
	dest_list = x;
	mac = &x->mac;
	return 0;
}
/**
 * @brief Send an IP packet to specified host. 
 *
 * @param src Source IP address.
 * @param dest Destination IP address.
 * @param proto Value of `protocol` field in IP header.
 * @param buf pointer to IP payload
 * @param len Length of IP payload
 * @return 0 on success, -1 on error.
 */
int sendIPPacket(const struct in_addr src, const struct in_addr dest, 
    int proto, const void *buf, int len){
	struct ether_addr dmac;
	if (get_mac_address(dest, &dmac)) {
		return -1;
	}
}



/** 
 * @brief Process a IP packet upon receiving it.
 *
 * @param buf Pointer to the packet.
 * @param len Length of the packet.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
typedef int (*IPPacketReceiveCallback)(const void* buf, int len);

/**
 * @brief Register a callback function to be called each time an IP packet
 * was received.
 *
 * @param callback The callback function.
 * @return 0 on success, -1 on error.
 * @see IPPacketReceiveCallback
 */
int setIPPacketReceiveCallback(IPPacketReceiveCallback callback);

/**
 * @brief Manully add an item to routing table. Useful when talking with real 
 * Linux machines.
 * 
 * @param dest The destination IP prefix.
 * @param mask The subnet mask of the destination IP prefix.
 * @param nextHopMAC MAC address of the next hop.
 * @param device Name of device to send packets on.
 * @return 0 on success, -1 on error
 */
int setRoutingTable(const struct in_addr dest, const struct in_addr mask, 
    const void* nextHopMAC, const char *device);
#endif
