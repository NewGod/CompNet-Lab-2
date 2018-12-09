#include<cstdio>
#include<iostream>
#include<string>
#include<cstring>
#include<cstdint>
using namespace std;
int main(){ 
	string s("192.168.0.1");
	uint8_t s_byte[4];
	sscanf(s.c_str(), "%hhu.%hhu.%hhu.%hhu", s_byte, s_byte+1, s_byte+2, s_byte+3); 
	printf("%d.%d.%d.%d\n", s_byte[0], s_byte[1], s_byte[2], s_byte[3]);
	return 0;
}
