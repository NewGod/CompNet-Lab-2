#include "device.h"
#include "proto.h"

#include <list>
#include <memory>
#include <pcap.h>

void Device::callback(const struct pcap_pkthdr *h, const u_char *bytes) {
    if (exit_flag) pcap_breakloop(pcap);
    if (h->caplen != h->len) return;
    eth_header *hdr = (eth_header*)bytes;
    if (hdr->h_dest != eth || hdr->h_dest != broadcast_eth) { 
        return;
    }else {
        size_t sz = h->len - sizeof(eth_header);
        std::vector<uint8_t> tmp(sz, 0);
        memcpy(tmp.data(), bytes + sizeof(eth_header), sz);
        switch (hdr->h_proto){
            case ETH_TYPE_IP:
                ip->push(std::make_pair(this, tmp));
                break;
            case ETH_TYPE_ARP:
                arp->push(std::make_pair(this, tmp));
                break;
            case ETH_TYPE_ROUTER:
                route->push(std::make_pair(this, tmp));
                break;
            default:
                break;
        }
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
    reinterpret_cast<Device*>(user[0])->callback(b, bytes);
}, reinterpret_cast<u_char*>(this));
    }
}

static int dev_cnt;
Device::Device(const std::string &name, SafeQueue<pairDevBuf> *ip, SafeQueue<pairDevBuf> *arp, SafeQueue<pairDevBuf> *route): ip(ip),arp(arp),route(route),name(name),id(++dev_cnt){
    exit_flag = 0;
    Receiver = std::thread(std::bind(&Device::ReceiveFunc, this, std::placeholders::_1), std::ref(exit_flag));
}    
Device::~Device(){
    exit_flag = 1;
    Receiver.detach();
}
 void Device::sendFrame(const void* buf,const size_t &len,const int &ethtype){
	//ethernet II  packing 
    size_t size = std::max<size_t>(sizeof(eth_header)+len+4, 64);
    std::vector<uint8_t> buffer(size, 0);
	void* eth = buffer.data();
	eth_header *eth_hdr = (eth_header*)eth;

    eth_hdr->h_dest = next_hop_eth;
    eth_hdr->h_source = eth;
	eth_hdr->h_proto = ethtype;
	memcpy((void*)((char*)eth + sizeof(eth_header)), buf, len); 

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
	if (pcap_inject(pcap, &eth, size)==-1) {
		pcap_perror(pcap,0);
        throw "sendFrame: Error";
	}
}

Device* DeviceManage::addDevice(const std::string &name) {
    try{ 
        Device *dev = findDevice(name);
        return dev;
    }catch (const char* msg){
        list.push_back(new Device(name, &ip, &arp, &route));
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
    findAllDevice();
}
pairDevBuf DeviceManage::getIpFrame(){
    return ip.get();
}
pairDevBuf DeviceManage::getArpFrame(){
    return arp.get();
}
pairDevBuf DeviceManage::getRouteFrame(){
    return route.get();
}
