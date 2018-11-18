#include<stdint.h>
#include<stdio.h>
struct eth_hdr
{
    unsigned char dmac[6];
    unsigned char smac[6];
    uint16_t ethertype;
} __attribute__((packed));

int main(){
	struct eth_hdr tmp;
	printf("%d\n", (int)sizeof(tmp));
}

