#include "ip.h"
#include "csapp.h"
#include <cstdio>
#include <netinet/ether.h>
#include <arpa/inet.h>
extern DeviceManage DeviceManager;
extern Ip Iper;

int main(){ 
	for (auto x: DeviceManager.list) {
			printf("name: %s id: %d ip: %s ", x->name.c_str(), x->id, inet_ntoa(*((in_addr*)&x->ip))); 
			printf("ip mask: %s mac: %s\n", inet_ntoa(*((in_addr*)&x->ip_mask)), ether_ntoa((ether_addr*)&x->eth));
		}
	while (true) {
		try{
			Buffer buf = Iper.getPacket();
			printf("%s\n", buf.data());
		}
		catch (const char* e) {
			printf("%s\n", e);
		}
	
	}
	return 0;
}
