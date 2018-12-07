#include "route.h"
#include "device.h"
#include "proto.h"
#include <thread>
#include <stdlib.h>
void Route::routeProtoReceiver(bool &exit_flag){
    while (!exit_flag) {
        std::pair<Device*, std::vector<uint8_t>> ret = DeviceManage.getRouteFrame();
        Device* dev = ret.first;
        std::vector<uint8_t> buffer = ret.second;
        RTHeader* header = (RTHeader*)buffer.data();
        dev->next_hop_eth = header->mac;
        uint8_t* st = buffer.data() + sizeof(RTHeader), *ed = st + header->sz * RTILen;
        for (uint8_t* i = st; i < ed; i+=RTILen) {
            bool flag = 0;
            ip_addr ip((void*)i), ip_mask((void *)(i+4));
            for (auto x : route_table) 
                if (x.ip == ip && ip_mask == ip) {
                    flag = 1;
                    break;
                }
            if (!flag) route_table.push_back(RTI(ip, ip_mask, dev));
        }
    }
}
void Route::routeProtoSender(bool &exit_flag){
    while (!exit_flag) { 
        size_t sz = sizeof(RTHeader) + (route_table.size()-1) * RTILen;
        std::vector<uint8_t> buffer;
        buffer.resize(sz, 0);
        RTHeader *header = (RTHeader*)buffer.data();
        header->ip = getMyIp();
        header->ip_mask = getMyIpMask();
        header->sz = sz;
        int cnt = 1;
        for (uint8_t* i = buffer.data() + sizeof(RTHeader); i < buffer.data() + sz; i += sz, cnt++){
            *(int*)i = route_table[cnt].ip;
            *(int*)(i+4) = route_table[cnt].ip_mask;
        }
        for (auto &dev : DeviceManage.list) {
            header->mac = dev->eth;
            dev->sendFrame(header, sz, ETH_TYPE_ROUTER);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
RTI Route::getRTI(ip_addr &ip){
    RTI ans = route_table[0];
    for (auto x: route_table) 
        if (x.in(ip)) ans = std::max(ans, x);
    return ans;
}

