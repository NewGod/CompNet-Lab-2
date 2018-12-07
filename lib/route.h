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
    Device *dev;
    RouteTableItem()=default;
    RouteTableItem(const ip_addr &ip, const ip_addr &ip_mask,Device *dev): ip(ip), ip_mask(ip_mask), dev(dev){}
    const bool in(ip_addr &x){
        return (ip_mask ^ x) == (ip^ip_mask);
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
    static bool exit_flag;
    static std::vector<RTI> route_table;
    static void routeProtoReceiver(bool &exit_flag);
    static void routeProtoSender(bool &exit_flag);
    static std::thread sender;
    static std::thread receiver;
public: 
    Route(){
        exit_flag = false;
        using namespace std::placeholders;
        sender = std::thread(routeProtoSender, std::ref(exit_flag));
        receiver = std::thread(routeProtoReceiver, std::ref(exit_flag));
        route_table.push_back(RTI(ip_addr(0),ip_addr(0),DeviceManage.getDevice()));
        return;
    }
    static RTI getRTI(ip_addr &ip);
    ~Route(){
        exit_flag = true;
        sender.join();
        receiver.join();
    }
}Router;
#endif
