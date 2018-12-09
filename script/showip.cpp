#include<cstdio>
#include<iostream>
#include<arpa/inet.h>
#include<netinet/ether.h>
#include<ifaddrs.h>
#include<cstring>
#include<sys/types.h>
#include<netpacket/packet.h>
using namespace std;
void getIPAddress(){
    string ipAddress="Unable to get IP Address";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
			switch (temp_addr->ifa_addr->sa_family) {
			case AF_INET: 
			printf("%s %s", temp_addr->ifa_name, inet_ntoa(((sockaddr_in*)temp_addr->ifa_addr)->sin_addr));
			printf("%s ",inet_ntoa(((sockaddr_in*)temp_addr->ifa_netmask)->sin_addr));
			printf("%s\n",inet_ntoa(((sockaddr_in*)temp_addr->ifa_dstaddr)->sin_addr));
			break;
			case AF_PACKET:
				printf("%s %s\n", temp_addr->ifa_name, ether_ntoa((ether_addr*)((sockaddr_ll*)temp_addr->ifa_addr)->sll_addr));
				break;
			default:
				printf("%s %d\n", temp_addr->ifa_name, temp_addr->ifa_addr->sa_family);
			 }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ;
}
int main() {
	getIPAddress();
}
