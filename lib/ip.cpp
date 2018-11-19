#include "ip.c"
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
	memcpy(arp.arp_eth_source, &broadcast_addr.mac, ETH_ALEN);
	memcpy(arp.arp_eth_source, &device->mac, ETH_ALEN);

	eachDevice([&arp](Device *device) {
		arp.arp_ip_source = device->ip;
		arp.arp_ip_dest = ip.s_addr;
		sendFrame(&arp, sizeof(arp), ETH_TYPE_ARP, arp.arp_eth_source, device);
	});

	char a[MAX_PCAP_FILTER_LEN]; 
	char src[MAX_IPV4_LEN];

	inet_pton(AF_INET,src,arp.arp_ip_dest);
	sprintf(a, "arp src %s", src);
	auto tmp = getSingleFrame(a, &arp);
    if (tmp.first == -1) return -1;
    arp = tmp.second

	Destination* x = malloc(sizeof(Destination));
	memcpy(&x->ip, &arp.arp_ip_source, sizeof(x->ip));
	memcpy(&x->mac, arp.arp_eth_source, sizeof(x->mac));
	x->nxt = dest_list;
	dest_list = x;
	mac = &x->mac;
	return 0;
}
int sendIPPacket(const struct in_addr src, const struct in_addr dest, 
    int proto, const void *buf, int len){
	in_addr nxt_hop_ip = getRouteTableItem(src); 
	nxt_hop_device, nxt_hop_mac = get_mac_address(nxt_hop_ip);
	
	buffer = buildIPFrame(src, dest, proto, buf);

	if (sendFrame(buffer, device, nxt_hop_mac) --;
	// 从 route table 获取下一跳的设备 

	// IP 地址可能不存在 mac table 上

	// 下一跳的 IP， (IP地址 + 子网掩码)
}

int getIPFrame(ip, callback){
	std::vector<uint8_t> buffer;
	header
	int n;
	if (getEthFrame(1, [&n, &header, &buffer] {
		...
	}) == -1)
		return -1;						
	if (getEthFrame(n, [&buffer] (...) {
		buffer = buffer + chars; 
	}) == -1)
		return -1;
	check_IPFrame(...);
	return -1;
	callback(..., buffer);	
	return ;
}

int getEthFrame(n, eth, callback) {
	pcap_loop(...)
	check_EthFrame(...);
	callback(...);
}

