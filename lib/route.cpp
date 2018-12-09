#include "route.h"
#include "arp.h"
#include "device.h"
#include "proto.h"
#include <thread>
#include <stdlib.h>
void Route::routeProtoReceiver(bool &exit_flag){
    while (!exit_flag) {
        std::pair<Device*, std::vector<uint8_t>> ret;
		try{
			ret = DeviceManager.getRouteFrame();
		}catch (const char* e) {
			continue;
		}
        Device* dev = ret.first;
        std::vector<uint8_t> buffer = ret.second;
        RTHeader* header = (RTHeader*)buffer.data();
		dev->next_hop_ip = header->ip;
        uint8_t* st = buffer.data() + sizeof(RTHeader), *ed = st + header->sz;
        for (uint8_t* i = st; i < ed; i+=RTILen) {
            bool flag = 0;
            ip_addr ip((void*)i), ip_mask((void *)(i+4));
            for (auto x : route_table) 
                if (x.ip == ip && x.ip_mask == ip_mask) {
                    flag = 1;
                    break;
                }
            if (!flag) {
				printf("Find ip: %s ip mask: %s dev: %s\n", ((std::string)ip).c_str(), ((std::string)ip_mask).c_str(), dev->name.c_str());
				route_table.push_back(RTI(ip, ip_mask, dev));
			}
        }
		if (header->type){
			std::unique_lock<std::mutex> lock(m);
			run = 1;
		}
    }
}
void Route::routeProtoSender(bool &exit_flag){
	bool first = 1;
    while (!exit_flag) { 
		bool flag; 
		{
			std::unique_lock<std::mutex> lock(m);
			flag = run;
			run = 0;
		}
		if (!flag){
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}
        size_t sz = sizeof(RTHeader) + (route_table.size()-1+DeviceManager.list.size()) * RTILen;
        std::vector<uint8_t> buffer;
        buffer.resize(sz, 0);
        RTHeader *header = (RTHeader*)buffer.data();
		header->sz = sz; 
		header->type = first;
        int cnt = 1;
		uint8_t* i = buffer.data() + sizeof(RTHeader);
        for (auto &dev : DeviceManager.list) {
            *(int*)i = dev->ip;
            *(int*)(i+4) = dev->ip_mask;
			i += sz;
        }
        for (; i < buffer.data() + sz; i += sz, cnt++){
            *(int*)i = route_table[cnt].ip;
            *(int*)(i+4) = route_table[cnt].ip_mask;
        }
		for (auto &dev : DeviceManager.list) {
            header->ip = dev->ip;
			eth_addr eth = broadcast_eth; 
			if (first) 
				dev->sendFrame(eth, header, sz, ETH_TYPE_ROUTER);
			else {
				if (int(dev->next_hop_ip) == 0) continue;
				eth = Arper.getMacAddr(dev->next_hop_ip);
				dev->sendFrame(eth, header, sz, ETH_TYPE_ROUTER);
			}
		}
		first = 0;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
RTI Route::getRTI(ip_addr &ip){
    RTI ans = route_table[0];
    for (auto x: route_table) 
        if (x.in(ip)) ans = ans>x?ans:x;
    return ans;
}
Route Router;
