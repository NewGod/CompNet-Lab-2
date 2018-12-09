#ifndef __ADDRESS_H__
#define __ADDRESS_H__
#include<cstdio>
#include<cstdint>
#include<cstring>
#include<string>
#include<vector>
#define IP_ALEN 4
typedef std::vector<uint8_t>  Buffer;
#pragma pack(push, 1)
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
    ip_addr(const int &s_addr):s_addr(s_addr) {}
    ip_addr(const void* s){
        memcpy(s_byte, s, IP_ALEN);
    }
	ip_addr(const std::string &s) {
		sscanf(s.c_str(), "%hhu.%hhu.%hhu.%hhu", s_byte, s_byte+1, s_byte+2, s_byte+3); 
	}
    operator int32_t(){ return s_addr; }
	operator std::string(){
		char tmp[16];
		sprintf(tmp, "%d.%d.%d.%d", s_byte[0], s_byte[1], s_byte[2], s_byte[3]);
		return std::string(tmp);
	}
    bool operator == (const ip_addr &x)const {
		return s_addr == x.s_addr;
    }
	bool operator < (const ip_addr &x) const{
		return s_addr < x.s_addr;
	}
};
#pragma pack(pop)

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
                return 1;
        return 0;
    }
    bool operator == (const eth_addr &x){
        for (int i=0;i<ETH_ALEN;i++) 
            if (s_byte[i] != x.s_byte[i]) 
                return 0;
        return 1;
    }
};
#pragma pack(pop)
static eth_addr broadcast_eth("\xff\xff\xff\xff\xff\xff");
static eth_addr eth_null("\x00\x00\x00\x00\x00\x00");
inline uint16_t rev_16(uint16_t x){
	return (x >> 8) | ((x & 0xff) << 8);
}
#endif 
