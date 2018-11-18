/** 
 * @file device.h
 * @brief Library supporting network device management.
 */
#ifndef __DEVICE_H__
#define __DEVICE_H__
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<net/ethernet.h>
typedef struct Dev{ 
	const char *name;
	int id;
	in_addr_t ip; 
	struct ether_addr mac;
	struct Dev* nxt;
}Dev ;
Dev *device_list;
int dev_cnt;

/**
 * Add a device to the library for sending/receiving packets. 
 *
 * @param device Name of network device to send/receive packet on.
 * @return An non-negative _device-ID_ on success, -1 on error.
 */
int addDevice(const char* device);

/**
 * Find a device added by `addDevice`.
 *
 * @param device Name of the network device.
 * @return An non-negative _device-ID_ on success, -1 if no such device 
 * was found.
 */
int findDevice(const char* device);

struct Dev* getDevice(int x);

int addDevice(const char* device) { 
	if (findDevice(device) != -1) return -1;
	Dev *d = (struct Dev*) malloc(sizeof(Dev));
	d->name = device;
	d->id = ++dev_cnt;
	d->nxt = device_list;
	device_list = d;
	return d->id;
}

int findDevice(const char* device) {
	for (struct Dev* i = device_list; i; i = i->nxt) 
		if (strcmp(device, i->name)) 
			return i->id;
	return -1;
}

struct Dev* getDevice(int x) {
	for (struct Dev* i = device_list; i; i = i->nxt) 
		if (i->id == x) 
			return i;
	return NULL;
}
#endif
