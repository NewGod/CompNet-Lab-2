#ifndef __ARP_H__
#define __ARP_H__
#include<map>
#include<mutex>
#include<thread>
#include "address.h"
#include "device.h"
#include "safequeue.h"
#define ARP_CACHE_SIZE 100
class Arp{
	SafeMap<ip_addr, eth_addr> req_buf;
    std::deque<Buf> arpCache;
    void ReceiveFunc(bool &exit_flag);
    std::thread Receiver;
    void sendArp(ip_addr ip);
    bool exit_flag;
    public:
    Arp();
    ~Arp();
    eth_addr getMacAddr(const ip_addr &ip);
};
extern Arp Arper;

#endif
