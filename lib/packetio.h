#ifndef __PACKETIO_H__
#define __PACKETIO_H__
/** 
 * @file packetio.h
 * @brief Library supporting sending/receiving Ethernet II frames.
 */
#include <thread>
#include "device.h"

class Ether{
    std::vector<void*> buffer;
    void frameReceiveFunc();
    std::thread frameReceiveMonitor;

    public:
    Ether();
    ~Ether();
    void sendFrame(const void* buf, const int &len, 
        const int &ethtype, const eth_addr &destmac, const Device &dev);

    Device* receiveRT(void *buf);
    Device* receiveARP(void *buf);
    Device* receiveIP(void *buf);
}Ether;

#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_TEST 0x9000

#endif
