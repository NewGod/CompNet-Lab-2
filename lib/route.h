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
    bool exit_flag, run;
    std::vector<RTI> route_table;
    void routeProtoReceiver(bool &exit_flag);
    void routeProtoSender(bool &exit_flag);
    std::thread sender;
    std::thread receiver;
    std::mutex m; 
public: 
    Route(){
        exit_flag = false;
		run = 1;
        using namespace std::placeholders;
        sender = std::thread(std::bind(&Route::routeProtoSender, this, std::placeholders::_1), std::ref(exit_flag));
        receiver = std::thread(std::bind(&Route::routeProtoReceiver, this, std::placeholders::_1), std::ref(exit_flag));
        route_table.push_back(RTI(ip_addr(0),ip_addr(0),DeviceManager.getDevice()));
        return;
    }
    RTI getRTI(ip_addr &ip);
    ~Route(){
        exit_flag = true;
        sender.join();
        receiver.join();
    }
};
extern Route Router;
#endif
