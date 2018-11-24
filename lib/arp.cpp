#include "arp.h"
#include "address.h"
#include "route.h"
#include "device.h"
#include "packetio.h"
void Arp::sendArp(ip_addr ip){
    Device dev = Router.getRTI(ip).dev;
	arp_t arp;
	arp.arp_hard_type = ARPHRD_ETHER;
	arp.arp_proto_type = 0x0800;
	arp.arp_hard_size = ETH_ALEN;
	arp.arp_proto_size = 4;
	arp.arp_op = ARP_OP_REQUEST;
    arp.arp_ip_source = getMyIp();
	arp.arp_eth_source = dev.eth;
    arp.arp_ip_dest = ip.s_addr;
	arp.arp_eth_dest = broadcast_addr;
    Ether.sendFrame(&arp, sizeof(arp), ETH_TYPE_ARP, arp.arp_eth_source, dev);
    return ; 
}
bool Arp::hasArp(ip_addr ip){
    return arpMapping.find(ip)!=arpMapping.end();
}
eth_addr Arp::getMacAddr(ip_addr ip){
    if (arpMapping.find(ip)!=arpMapping.end()) return arpMapping[ip];
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    std::condition_variable cv;
    sendArp(ip);
    if (arpRequest.find(ip)!=arpRequest.end()) cv = arpRequest[ip];
    arpRequest[ip] = cv;
    while (!hasArp(ip)){
        cv.wait_for(lck, hasArp, 1000);
    }
    return arpMapping[ip];
}
Arp::Arp(){ 
    exit_flag = 0;
    Receiver = std::thread(ReceiverFunc, std::ref(exist_flag));
}
Arp::~Arp(){
    exit_flag = 1;
    Receiver.detach();
}
Arp::ReceiveFunc(bool &exit_flag){
    while (!exit_flag){

    }
} 