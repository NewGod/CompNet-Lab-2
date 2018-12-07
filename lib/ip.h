#ifndef __PACKETIO_H__
#define __PACKETIO_H__
/** 
 * @file ip.h
 * @brief Library supporting sending/receiving IP packets encapsuled in an 
 * Ethernet II frame.
 */
#include "device.h"
#include "arp.h"
#include "route.h"
#include "address.h"
#include <string>
#include <thread>
#include <map>
#include <list>

class Ip{
    std::map<ip_addr, std::list<std::string>> buffer;
    void ReceiverFunc();
    std::thread Receiver;
    public: 
    Ip();
    ~Ip();
    void SendIPPacket(std::string);
    std::string ReceiveIPacket();
}Ip;
#endif
