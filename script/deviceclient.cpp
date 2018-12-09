#include "ip.h"
#include "csapp.h"
#include <cstdio>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
extern Ip Iper;

int main(){ 
	char buf[MAXLINE];
	sleep(1);
	while (Fgets(buf, MAXLINE, stdin)!=NULL) {
		Buffer b(strlen(buf));
		memcpy(b.data(), buf, strlen(buf));
		printf("%s\n", b.data());
		Iper.sendIPPacket(ip_addr(std::string("10.100.1.1")), 4, b);
	}
	return 0;
}
