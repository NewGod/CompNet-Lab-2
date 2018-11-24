#ifndef __ROUTE_H__
#define __ROUTE_H__
#include <cstdint>
#include <vector>
#include <thread>
#include "device.h"
#include "address.h"
class RouteTableItem{
public:
    ip_addr ip;
    ip_addr ip_mask;
    Device dev;
    RouteTableItem()=default;
    RouteTableItem(const ip_addr &ip, const ip_addr &ip_mask,const struct Device &dev): ip(ip), ip_mask(ip_mask), dev(dev){}
    const bool in(const ip_addr &x){
        return (x^ip_mask) == (ip^ip_mask);
    }
    const bool operator > (RouteTableItem &x){
        return ip_mask > x.ip_mask;
    }
    const bool operator < (RouteTableItem &x){
        return ip_mask < x.ip_mask;
    }
};
typedef RouteTableItem RTI;

class Route{ 
    bool exit_flag;
    std::vector<RTI> route_table;
    void routeProtoReceiver(bool &exit_flag);
    void routeProtoSender(bool &exit_flag);
    std::thread sender;
    std::thread receiver;
public: 
    Route(){
        exit_flag = false;
        sender = std::thread(routeProtoSender, std::ref(exit_flag));
        receiver = std::thread(routeProtoReceiver, std::ref(exit_flag));
        route_table.push_back(RTI(ip_addr(0),ip_addr(0),DeviceManage.getDevice()));
        return;
    }
    RTI getRTI(const ip_addr &ip);
    ~Route(){
        exit_flag = true;
        sender.join();
        receiver.join();
    }
}Route;
#pragma pack(push, 1) 
struct RTHeader{ 
    ip_addr ip;
    ip_addr ip_mask;
    eth_addr mac;
    uint8_t ttl;
    uint8_t cnt;
};
#pragma pack(pop)

#endif
