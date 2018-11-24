#include<stdint.h>
struct ip_addr{
    uint32_t s_addr;
    ip_addr(uint32_t s_addr):s_addr(s_addr){}
} __attribute__((packed));

int main(){
	ip_addr tmp(0);
	printf("%d\n", (int)sizeof(tmp));
}

