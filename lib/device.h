#ifndef __DEVICE_H__
#define __DEVICE_H__
/** 
 * @file device.h
 * @brief Library supporting network device management.
 */
#include "protocol.h"

#include <string>
#include <functional>

struct Device {
    std::string name;
    int id;
    in_addr ip;
    eth_addr eth;
    Device(const std::string &name);
};

/**
 * Add a device to the library for sending/receiving packets. 
 *
 * @param device Name of network device to send/receive packet on.
 * @return An non-negative _device-ID_ on success, -1 on error.
 */
int addDevice(const std::string &name);

/**
 * Find a device added by `addDevice`.
 *
 * @param device Name of the network device.
 * @return An non-negative _device-ID_ on success, -1 if no such device 
 * was found.
 */
Device *findDevice(const std::string &name);

Device *getDevice(int id);

void eachDevice(const std::function<void(Device*)> &callback);

#endif
