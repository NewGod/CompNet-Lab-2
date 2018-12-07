#include "arp.h"
#include "address.h"
#include "proto.h"
#include "route.h"
#include "device.h"
void Arp::sendArp(ip_addr ip){
    Device* dev = Router.getRTI(ip).dev;
	arp_t arp;
	arp.arp_hard_type = ARPHRD_ETHER;
	arp.arp_proto_type = 0x0800;
	arp.arp_hard_size = ETH_ALEN;
	arp.arp_proto_size = 4;
	arp.arp_op = ARP_OP_REQUEST;
    arp.arp_ip_source = getMyIp();
	arp.arp_eth_source = dev->eth;
    arp.arp_ip_dest = ip.s_addr;
	arp.arp_eth_dest = broadcast_eth;
    dev->sendFrame(&arp, sizeof(arp), ETH_TYPE_ARP);
    return ; 
}
bool Arp::hasArp(ip_addr ip){
    return arpMapping.find(ip)!=arpMapping.end();
}
eth_addr Arp::getMacAddr(ip_addr ip){
    if (arpMapping.find(ip)!=arpMapping.end()) return arpMapping[ip];
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable *cv;
    sendArp(ip);

    if (arpRequest.find(ip)!=arpRequest.end()) cv = arpRequest[ip];
    else cv = new std::condition_variable();
    arpRequest[ip] = cv;
    while (!hasArp(ip)) cv->wait(lck);
    return arpMapping[ip];
}
Arp::Arp(){ 
    exit_flag = 0;
    Receiver = std::thread(std::bind(&Arp::ReceiveFunc, this, std::placeholders::_1), std::ref(exit_flag));
}
Arp::~Arp(){
    exit_flag = 1;
    Receiver.detach();
}
void Arp::ReceiveFunc(bool &exit_flag){
    while (!exit_flag){
        pairDevBuf tmp = DeviceManage.getArpFrame();
        Device* dev = tmp.first;
        Buf buf = tmp.second;
        arp_t* hdr = (arp_t*)buf.data();
        
        bool flag = 0;
        for (auto & x : arpCache)
            if (x == buf) {
                flag = 1;
                break;
            }
        if (flag) continue;
        arpCache.push_back(buf);
        if (arpCache.size() > ARP_CACHE_SIZE) arpCache.pop_front();
        arpMapping[hdr->arp_ip_source] = hdr->arp_eth_source;
        if (arpRequest.find(hdr->arp_ip_source) != arpRequest.end()) 
            arpRequest[hdr->arp_ip_source]->notify_all();
        if (hdr->arp_ip_dest == getMyIp()) {
            arp_t arp;
            arp.arp_hard_type = ARPHRD_ETHER;
            arp.arp_proto_type = 0x0800;
            arp.arp_hard_size = ETH_ALEN;
            arp.arp_proto_size = 4;
            arp.arp_op = ARP_OP_REPLY;
            arp.arp_ip_source = getMyIp();
            arp.arp_eth_source = dev->eth;
            arp.arp_ip_dest = hdr->arp_ip_source;
            arp.arp_eth_dest = hdr->arp_eth_dest;
            dev->sendFrame(&arp, sizeof(arp), ETH_TYPE_ARP);
        }else {
            Device* dev = Router.getRTI(hdr->arp_ip_dest).dev;
            dev->sendFrame(hdr, sizeof(arp_t), ETH_TYPE_ARP);
        }
    }
} 
