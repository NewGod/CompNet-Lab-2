#ifndef __DEVICE_H__
#define __DEVICE_H__
/** 
 * @file device.h
 * @brief Library supporting network device management.
 */
#include "proto.h"
#include "safequeue.h"

#include <cstring>
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
class Device;
typedef std::pair<Device*, std::vector<uint8_t>> pairDevBuf;
typedef std::vector<uint8_t> Buf;
class Device {
    SafeQueue<pairDevBuf> *Ip, *Arp, *Route;
    void callback(const struct pcap_pkthdr *h, const u_char *bytes);
    void ReceiveFunc(bool &exit_flag);
    std::thread Receiver;
    bool exit_flag;
    pcap_t *pcap;

    public:
    std::string name;
    int id;
    eth_addr eth;
	ip_addr ip, ip_mask;
	ip_addr next_hop_ip;
    Device(const std::string &name, SafeQueue<pairDevBuf> *ip, SafeQueue<pairDevBuf> *arp, SafeQueue<pairDevBuf> *route);
    ~Device();
    void sendFrame(const eth_addr& dest, const void* buf, const size_t &len, 
        const int &ethtype);
    //std::vector<uint8_t> getFrame(int proto);
};

class DeviceManage{
    SafeQueue<pairDevBuf> Ip, Arp, Route;
    void findAllDevice();
    public:
    std::vector<Device*> list;
    DeviceManage();
    ~DeviceManage(){
        for (auto &i :list) delete i;
    }
    /**
     * Add a device to the library for sending/receiving packets. 
     *
     * @param device Name of network device to send/receive packet on.
     * @return An non-negative _device-ID_ on success, -1 on error.
     */
    Device* addDevice(const std::string &name);
    /**
     * Find a device added by `addDevice`.
     *
     * @param device Name of the network device.
     * @return An non-negative _device-ID_ on success, -1 if no such device 
     * was found.
     */
    Device* findDevice(const std::string &name); 
    Device* getDevice(int id=0);
    void eachDevice(const std::function<void(Device*)> &callback);
    pairDevBuf getIpFrame();
    pairDevBuf getArpFrame();
    pairDevBuf getRouteFrame();
	bool isMyIp(const ip_addr& ip) {
		for (auto x:list) 
			if (x->ip == ip) return true; 
		return false;
	}
};
extern DeviceManage DeviceManager;

#endif
