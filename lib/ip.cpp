#include "ip.h"
#include "proto.h"
void Ip::sendIPPacket(ip_addr dest, int proto, Buffer payload){
	RTI rti = Router.getRTI(dest); 
	int len = payload.size() + IP_HDR_SIZE;
	ip_header hdr; 
	Buffer buf(len, 0);
	hdr.len = rev_16(len);
	hdr.proto = proto;
	hdr.sour_addr = rti.dev->ip; 
	hdr.dest_addr = dest; 
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	hdr.ident = (uint16_t)ms;
	memcpy(buf.data(), &hdr, IP_HDR_SIZE);
	memcpy(buf.data()+IP_HDR_SIZE, payload.data(), payload.size());
	eth_addr eth = Arper.getMacAddr(rti.dev->next_hop_ip);
	rti.dev->sendFrame(eth, buf.data(), len, ETH_TYPE_IP);
	return;
}

void Ip::ReceiveFunc(bool &exit_flag){ 
	while (!exit_flag) { 
		pairDevBuf tmp;
		try { 
			tmp = DeviceManager.getIpFrame();
		}catch(const char* e) {
			continue;
		}
		Device *dev = tmp.first;
		Buffer &buf = tmp.second;
		ip_header* hdr = (ip_header*)buf.data(); 
		if (DeviceManager.isMyIp(hdr->dest_addr)) { // not allow unorder frame
			if (hdr->getFragment() != 0) {
				Buffer tmp_buf = req_buf.Get(hdr->ident); 
				if (tmp_buf.size() < IP_HDR_SIZE) continue;
				ip_header *tmp_hdr = (ip_header*)tmp_buf.data();
				if (!tmp_hdr->getMF()) continue; 
				if (tmp_hdr->len - tmp_hdr->getIHL()!= hdr->getFragment()) continue; 
				size_t old_len = tmp_hdr->len;
				tmp_hdr->len += hdr->len - hdr->getIHL();
				
				tmp_buf.resize(tmp_hdr->len);
				tmp_hdr = (ip_header*)tmp_buf.data();
				memcpy(tmp_buf.data()+old_len, buf.data()+hdr->getIHL(), tmp_hdr->len - old_len);
				tmp_hdr->setMF(hdr->getMF());
				hdr = tmp_hdr;
			}
			if (!hdr->getMF()) {
				size_t sz = hdr->len - hdr->getIHL(); 
				Buffer ret(sz, 0);
				memcpy(ret.data(), buf.data() + hdr->getIHL(), sz);
				que.push(ret);
			}
			else req_buf.Set(hdr->ident, buf);
		}else {
			if (hdr->ttl == 0) continue; 
			hdr->ttl --;
			RTI rti = Router.getRTI(hdr->dest_addr); 
			eth_addr eth = Arper.getMacAddr(dev->next_hop_ip);
			rti.dev->sendFrame(eth, buf.data(), hdr->len, ETH_TYPE_IP);
		}
	}
}

Buffer Ip::getPacket(){ 
	return que.get();
}

Ip::Ip() { 
	exit_flag = 0;
    Receiver = std::thread(std::bind(&Ip::ReceiveFunc, this, std::placeholders::_1), std::ref(exit_flag));
}
Ip::~Ip(){ 
	exit_flag = 1; 
	Receiver.join();
}
Ip Iper;
