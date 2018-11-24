#include<cstdint>
#include<cstdio>
#include<cstring>
#define ETH_ALEN 6
#pragma pack(push, 1)
struct eth_addr{
    union {        
        struct { 
            uint8_t s_byte[ETH_ALEN];
        };
    };          
    eth_addr()=default;
    eth_addr(const void * s) {
        memcpy(s_byte, s, sizeof(s_byte));
    }
    bool operator = (const void *s){
        memcpy(s_byte, s, sizeof(s_byte));
        return 1;
    }
    bool operator != (const eth_addr &x){
        for (int i=0;i<ETH_ALEN;i++) 
            if (s_byte[i] != x.s_byte[i]) 
                return 0;
        return 1;
    }
};
#pragma pack(pop)

int main(){
	eth_addr tmp;
    uint8_t s_byte[ETH_ALEN];
	printf("%d %d\n", (int)sizeof(tmp), (int) sizeof(s_byte));
}

