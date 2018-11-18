/** 
 * @file packetio.h
 * @brief Library supporting sending/receiving Ethernet II frames.
 */

#ifndef __PACKETIO_H__
#define __PACKETIO_H__
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <stdint.h>
#include <stdio.h>
//#include <libpcap.h>
#define ETH_TYPE_IPV4 0x0800
#define ETH_TYPE_ARP 0x0806

/** 
 * @brief Encapsule some data into an Ethernet II frame and send it.
 *
 * @param buf Pointer to the payload.
 * @param len Length of the payload.
 * @param ethtype EtherType field value of this frame.
 * @param destmac MAC address of the destination.
 * @param id ID of the device(returned by `addDevice`) to send on.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
int sendFrame(const void* buf, int len, 
    int ethtype, const void* destmac, int id);

/** 
 * @brief Process a frame upon receiving it.
 *
 * @param buf Pointer to the frame.
 * @param len Length of the frame.
 * @param id ID of the device(returned by `addDevice`) receiving current 
 * frame.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
typedef int (*frameReceiveCallback)(const void*, int, int);

/**
 * @brief Register a callback function to be called each time an Ethernet II 
 * frame was received.
 *
 * @param callback The callback function.
 * @return 0 on success, -1 on error.
 * @see frameReceiveCallback
 */
int setFrameReceiveCallback(frameReceiveCallback callback);

#endif
