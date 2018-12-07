#ifndef __ARP_H__
#define __ARP_H__
#include<map>
#include<mutex>
#include<thread>
#include "address.h"
#include "device.h"
#define ARP_CACHE_SIZE 100
class Arp{
    std::map<ip_addr, std::condition_variable*> arpRequest;
    std::map<ip_addr, eth_addr> arpMapping;
    std::deque<Buf> arpCache;
    void ReceiveFunc(bool &exit_flag);
    std::thread Receiver;
    void sendArp(ip_addr ip);
    bool hasArp(ip_addr ip);
    bool exit_flag;
    public:
    Arp();
    ~Arp();
    eth_addr getMacAddr(ip_addr ip);
};

#endif
