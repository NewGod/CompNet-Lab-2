#include "device.h"
#include "proto.h"

#include <list>
#include <memory>
#include <pcap.h>
#include <iostream>
#include <netpacket/packet.h>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>

void Device::callback(const struct pcap_pkthdr *h, const u_char *bytes) {
    if (exit_flag) pcap_breakloop(pcap);
    if (h->caplen != h->len) return;
    eth_header *hdr = (eth_header*)bytes;
	size_t sz = h->len - sizeof(eth_header);
	std::vector<uint8_t> tmp(sz, 0);
	memcpy(tmp.data(), bytes + sizeof(eth_header), sz);

	if (hdr->h_source == eth) return;
    if (hdr->h_dest != eth && hdr->h_dest != broadcast_eth) return;
	uint16_t proto = rev_16(hdr->h_proto);
	switch (proto){
		case ETH_TYPE_IP:
			Ip->push(std::make_pair(this, tmp));
			break;
		case ETH_TYPE_ARP:
			Arp->push(std::make_pair(this, tmp));
			break;
		case ETH_TYPE_ROUTER:
			Route->push(std::make_pair(this, tmp));
			break;
		default:
			break;
	}
    return;
}

void Device::ReceiveFunc(bool &exit_flag){
    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    pcap_errbuf[0]='\0';
    
    pcap=pcap_open_live(name.c_str(), 96, 0, 0, pcap_errbuf);
    struct Guard {
        pcap_t *pcap;
        ~Guard() {
        }
    } guard{pcap};
    if (pcap_errbuf[0]!='\0') 
        fprintf(stderr,"%s",pcap_errbuf);

    if (!pcap) throw "ReceiveFunc: Error";
    while (!exit_flag) { 
        using namespace std::placeholders;
        pcap_loop(pcap, -1,[](u_char *user, const pcap_pkthdr *b, const u_char *bytes) {
    reinterpret_cast<Device*>(user)->callback(b, bytes);
}, reinterpret_cast<u_char*>(this));
    }
}

static int dev_cnt;
Device::Device(const std::string &name, SafeQueue<pairDevBuf> *Ip, SafeQueue<pairDevBuf> *Arp, SafeQueue<pairDevBuf> *Route): Ip(Ip),Arp(Arp),Route(Route),name(name),id(dev_cnt++){
	next_hop_ip = ip_addr(0);
    exit_flag = 0;
    Receiver = std::thread(std::bind(&Device::ReceiveFunc, this, std::placeholders::_1), std::ref(exit_flag));
}    
Device::~Device(){
    exit_flag = 1;
	pcap_breakloop(pcap);
    Receiver.join();
}
 void Device::sendFrame(const eth_addr &dest, const void* buf,const size_t &len, const int &ethtype){
	//ethernet II  packing 
    size_t size = std::max<size_t>(sizeof(eth_header)+len+4, 64);
    std::vector<uint8_t> buffer(size, 0);
	void* ptr = buffer.data();
	eth_header *eth_hdr = (eth_header*)ptr;

	if (eth_addr(dest) != broadcast_eth && int(next_hop_ip) == 0) { 
		throw "sendFrame: Target Device Not Found";
	}
	eth_hdr->h_dest = dest;
    eth_hdr->h_source = eth;
	eth_hdr->h_proto = rev_16(ethtype);
	memcpy((void*)((char*)ptr + sizeof(eth_header)), buf, len); 

    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    pcap_errbuf[0]='\0';
    
    pcap_t* pcap=pcap_open_live(name.c_str(), 96, 0, 0, pcap_errbuf);
    if (!pcap) throw "sendFunc: Error";
	//checksum needed
    struct Guard {
        pcap_t *pcap;
        ~Guard() {
        }
    } guard{pcap};
	if (pcap_inject(pcap, buffer.data(), size)==-1) {
		pcap_perror(pcap,0);
        throw "sendFrame: Error";
	}
}

Device* DeviceManage::addDevice(const std::string &name) {
    try{ 
        Device *dev = findDevice(name);
        return dev;
    }catch (const char* msg){
        list.push_back(new Device(name, &Ip, &Arp, &Route));
        return list.back();
    }
}
Device* DeviceManage::findDevice(const std::string &name) {
	for (auto &i : list)
        if (i->name == name)
            return i;
    throw "findDevice: Device Not Find!";
}
Device* DeviceManage::getDevice(int id) {
    for (auto &i : list)
        if (i->id == id)
            return i;
    throw "getDevice: Device Not Find!";
}

void DeviceManage::eachDevice(const std::function<void(Device*)> &callback) {
    for (auto &i : list)
        callback(i);
}

DeviceManage::DeviceManage(){
	struct ifaddrs *interfaceArray = NULL, *tempIfAddr = NULL;

	if (getifaddrs(&interfaceArray) < 0) {
		std::cerr << "getifaddrs() failed:" << strerror(errno) << std::endl;
		exit(-1);
	}

	for(tempIfAddr = interfaceArray; tempIfAddr != NULL; tempIfAddr = tempIfAddr->ifa_next)
	{
		if (!strcmp(tempIfAddr->ifa_name, "lo")) continue;
		Device* dev = addDevice(tempIfAddr->ifa_name);

		switch (tempIfAddr->ifa_addr->sa_family){
			case AF_INET:
				dev->ip = ((struct sockaddr_in *)tempIfAddr->ifa_addr)->sin_addr.s_addr;
				dev->ip_mask = ((struct sockaddr_in *) tempIfAddr->ifa_netmask)->sin_addr.s_addr;
				break;

			case AF_PACKET:
				dev->eth = ((struct sockaddr_ll*) tempIfAddr->ifa_addr)->sll_addr;
				break;
		}
	}
	freeifaddrs(interfaceArray);             /* free the dynamic memory */
	interfaceArray = NULL;                   /* prevent use after free  */
}
pairDevBuf DeviceManage::getIpFrame(){
    return Ip.get();
}
pairDevBuf DeviceManage::getArpFrame(){
    return Arp.get();
}
pairDevBuf DeviceManage::getRouteFrame(){
    return Route.get();
}
DeviceManage DeviceManager;
