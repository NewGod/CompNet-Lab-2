#ifndef __DEVICE_H__
#define __DEVICE_H__
/** 
 * @file device.h
 * @brief Library supporting network device management.
 */
#include "address.h"

#include <cstring>
#include <functional>
#include <vector>
#include <string>

struct Device {
    std::string name;
    int id;
    ip_addr ip;
    eth_addr eth;
    Device()=default;
    Device(const std::string &name);  
    Device(const Device &dev) {
        std::memcpy(this, &dev, sizeof(dev));
    }
};

class DeviceManage{
    std::vector<Device> list;
    public:
    /**
     * Add a device to the library for sending/receiving packets. 
     *
     * @param device Name of network device to send/receive packet on.
     * @return An non-negative _device-ID_ on success, -1 on error.
     */
    Device addDevice(const std::string &name);
    /**
     * Find a device added by `addDevice`.
     *
     * @param device Name of the network device.
     * @return An non-negative _device-ID_ on success, -1 if no such device 
     * was found.
     */
    Device findDevice(const std::string &name); 
    Device getDevice(int id=0);
    void eachDevice(const std::function<void(Device)> &callback);
}DeviceManage;
#endif
