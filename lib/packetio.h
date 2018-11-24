#ifndef __PACKETIO_H__
#define __PACKETIO_H__
/** 
 * @file packetio.h
 * @brief Library supporting sending/receiving Ethernet II frames.
 */
#include<therad>

#define ETHER_TEST 0x9000
class EtherManager{
    std::vector<void*> buffer;
    void frameReceiveFunc();
    thread::frameReceiveMonitor;

    public:
    EtherMAnager();
    ~EtherManager();
    int sendFrame(const void* buf, int len, 
        int ethtype, const void* destmac, int id);

    Device* receiveRT(void *buf);
    Device* receiveARP(void *buf);
    Device* receiveIP(void *buf);
};
#endif
