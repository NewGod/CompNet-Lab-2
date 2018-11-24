#ifndef __ADDRESS_H__
#define __ADDRESS_H__
#include<cstdint>
#include<cstring>
#define IP_ALEN 4
struct ip_addr{
    union {
        uint32_t s_addr;
        struct {
            uint8_t s_byte[4];
        };
        struct {
            uint16_t s_word[2];            
        };
    };
    ip_addr()=default;
    ip_addr(const uint32_t &s_addr):s_addr(s_addr) {}
    ip_addr(const void* s){
        memcpy(s_byte, s, IP_ALEN);
    }
    operator int32_t(){ return s_addr; }
};

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
static eth_addr broadcast_addr({"\xff\xff\xff\xff\xff\xff"});
#endif 
