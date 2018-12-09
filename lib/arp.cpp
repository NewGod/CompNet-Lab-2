#include "arp.h"
#include "address.h"
#include "proto.h"
#include "device.h"
#include <iostream>
void Arp::sendArp(ip_addr ip){
	for (auto &dev : DeviceManager.list) {
		arp_header arp;
		arp.arp_hard_type = rev_16(ARPHRD_ETHER);
		arp.arp_proto_type = rev_16(0x0800);
		arp.arp_hard_size = ETH_ALEN;
		arp.arp_proto_size = 4;
		arp.arp_opear = rev_16(ARP_OP_REQUEST);
		arp.arp_ip_source = dev->ip;
		arp.arp_eth_source = dev->eth;
		arp.arp_ip_dest = ip.s_addr;
		arp.arp_eth_dest = broadcast_eth;
		dev->sendFrame(broadcast_eth, &arp, sizeof(arp), ETH_TYPE_ARP);
	}
	return ; 
}
bool hasArp(eth_addr eth){
    return eth != eth_null;
}
eth_addr Arp::getMacAddr(const ip_addr &ip){
	if (int(ip_addr(ip)) == 0) 
		throw "Arp::getMacAddr error: No Ip Address";

	if (hasArp(req_buf.Get(ip))) return req_buf.Get(ip);
    sendArp(ip);

	return req_buf.Request(ip, hasArp);
}
Arp::Arp(){ 
    exit_flag = 0;
    Receiver = std::thread(std::bind(&Arp::ReceiveFunc, this, std::placeholders::_1), std::ref(exit_flag));
}
Arp::~Arp(){
    exit_flag = 1;
    Receiver.join();
}
void Arp::ReceiveFunc(bool &exit_flag){
    while (!exit_flag){
        pairDevBuf tmp;
		try {
			tmp = DeviceManager.getArpFrame();
		}catch (const char* e) {
			continue; 
		}
        Device* dev = tmp.first;
        Buf buf = tmp.second;
        arp_header* hdr = (arp_header*)buf.data();
        
        bool flag = 0;
        for (auto & x : arpCache)
            if (x == buf) {
                flag = 1;
                break;
            }
        if (flag) continue;
        arpCache.push_back(buf);
        if (arpCache.size() > ARP_CACHE_SIZE) arpCache.pop_front();
		req_buf.Set(hdr->arp_ip_source, hdr->arp_eth_source);
		req_buf.Notify(hdr->arp_ip_source);
        if (DeviceManager.isMyIp(hdr->arp_ip_dest)) {
			if (hdr->arp_opear == rev_16(ARP_OP_REPLY)) continue;
            arp_header arp;
            arp.arp_hard_type = rev_16(ARPHRD_ETHER);
            arp.arp_proto_type = rev_16(0x0800);
            arp.arp_hard_size = ETH_ALEN;
            arp.arp_proto_size = 4;
            arp.arp_opear = rev_16(ARP_OP_REPLY);
            arp.arp_ip_source = dev->ip;
            arp.arp_eth_source = dev->eth;
            arp.arp_ip_dest = hdr->arp_ip_source;
            arp.arp_eth_dest = hdr->arp_eth_dest;
			eth_addr eth = broadcast_eth; // need to prevent broadcast
            dev->sendFrame(eth, &arp, sizeof(arp), ETH_TYPE_ARP);
        }else {
			for (auto x : DeviceManager.list) {
				if (x != dev)
					dev->sendFrame(broadcast_eth, hdr, sizeof(arp_header), ETH_TYPE_ARP);
			}
        }
    }
	return ;
} 
Arp Arper;
