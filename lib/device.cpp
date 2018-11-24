#include "device.h"

#include <list>
#include <memory>

static int dev_cnt;

Device::Device(const std::string &name): name(name),id(++dev_cnt),ip(),eth(){}    

Device DeviceManage::addDevice(const std::string &name) {
    try{ 
        Device dev = findDevice(name);
        return dev;
    }catch (const char* msg){
        list.push_back(Device(name));
        return list.back();
    }
}
Device DeviceManage::findDevice(const std::string &name) {
	for (auto &i : list)
        if (i.name == name)
            return i;
    throw "findDevice: Device Not Find!";
}
Device DeviceManage::getDevice(int id) {
    if (id == 0) return list[0];
    for (auto &i : list)
        if (i.id == id)
            return i;
    throw "getDevice: Device Not Find!";
}

void DeviceManage::eachDevice(const std::function<void(Device)> &callback) {
    for (auto &i : list)
        callback(i);
}
