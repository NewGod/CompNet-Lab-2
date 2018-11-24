#include "route.h"
#include "device.h"
#include "packetio.h"
#include <thread>
#include <stdlib.h>
void Route::routeProtoReceiver(bool &exit_flag){
    header* header = malloc(MAX_BUFFER_LEN);
    while (!exit_flag) {
        Device dev = EtherManager.receiveRT(header);
        for (void* i = x + sizeof(RTHeader); i < x + sz; i+=8) {
            bool flag = 0;
            ip_addr ip(i), ip_mask((void*)i+4);
            for (auto x : route_table) 
                if (x.ip == ip && ip_mask == ip) {
                    flag = 1;
                    break;
                }
            route_table.push_back(RTI(ip, ip_mask, dev));
        }
    }
}
void Route::routeProtoSender(bool &exit_flag){
    while (!exit_flag) { 
        size_t sz = sizeof(RTHeader) + (route_table.len()-1) * sizeof(uint32_t)*2;
        void* x = malloc(sz);
        RTHeader *header = x;
        header->ip = getSelfIp();
        header->ip_mask = getSelfIpMask();
        header->sz = sz;
        int cnt = 1;
        for (void* i = x + sizeof(RTHeader); i < x + sz; i+=8, cnt++){
            *(int*)i = route_table[cnt].ip;
            *(int*)(i+4) = route_table[cnt].ip_mask;
        }
        DeviceManage.eachDevice([&header](Device device){
            header->mac = device.mac;
            EtherManager.send(header, header->sz, ETHER_TEST, broadcast_mac, device);
        });
        sleep(1);
    }
}
RTI Route::getRTI(const ip_addr &ip){
    RTI ans = route_table[0];
    for (auto x: route_table) 
        if (x.in(ip)) ans = std::max(ans, x);
    return ans;
}

