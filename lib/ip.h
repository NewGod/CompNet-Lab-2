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
#include "safequeue.h"
#include <string>
#include <thread>
#include <map>
#include <list>

class Ip{
	bool exit_flag;
	SafeMap<uint16_t, Buffer> req_buf;
	SafeQueue<Buffer> que;
    void ReceiveFunc(bool &exit_flag);
    std::thread Receiver;
    public: 
    Ip();
    ~Ip();
    void sendIPPacket(ip_addr, int proto, Buffer);
    std::string ReceiveIPacket();
	Buffer getPacket();
};
#endif
