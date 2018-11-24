#include "packetio.h"
#include "protocol.h"
/** 
 * @brief Encapsule some data into an Ethernet II frame and send it.
 *
 * @param buf Pointer to the payload.
 * @param len Length of the payload.
 * @param ethtype EtherType field value of this frame.
 * @param destmac MAC address of the destination.
 * @param id ID of the device(returned by `addDevice`) to send on.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
int EtherManager::sendFrame(const void* buf, int len, int ethtype, const eth_addr &destmac, int id){
	//ethernet II  packing 
    size_t size = std::max<size_t>(sizeof(eth_header)+len+4, 64);
    std::vector<uint8_t> buffer(size, 0);
	void* eth = buffer.data();
	eth_header *eth_hdr = (eth_header*)eth;
	Device* device = getDevice(id);

	memcpy(eth_hdr->h_dest, &destmac, ETH_ALEN);
	memcpy(eth_hdr->h_source, &device->mac, ETH_ALEN); 
	eth_hdr->ether_type = ethtype;
	memcpy(eth + sizeof(ether_header), buf, len); 

	//checksum needed
	char pcap_errbuf[PCAP_ERRBUF_SIZE];
	pcap_errbuf[0]='\0';
	
	pcap_t* pcap=pcap_open_live(device->name.c_str(),96,0,0,pcap_errbuf);
	if (pcap_errbuf[0]!='\0') 
		fprintf(stderr,"%s",pcap_errbuf);
	if (!pcap) {
		return -1;
	}
    struct Guard {
        pcap_t *pcap;
        ~Guard() {
            pcap_close(pcap);
        }
    } guard{pcap};
	if (pcap_inject(pcap, &eth, size)==-1) {
		pcap_perror(pcap,0);
		return -1;
	}
	return 0;
}

std::pair<int, std::vector<uint8_t>> getSingleFrame(const std::string &filter) { 
	char pcap_errbuf[PCAP_ERRBUF_SIZE];
	pcap_errbuf[0]='\0';
	
	pcap_t* pcap=pcap_open_live(if_name,96,0,0,pcap_errbuf);
	if (pcap_errbuf[0]!='\0') 
		fprintf(stderr,"%s",pcap_errbuf);

	if (!pcap) return { -1 };

	if(pcap_compile(descr, &fp, filter.c_st, 0, netp) == -1) {
        fprintf(stderr, "Error calling pcap_compile\n");
		return { -1 };
    } 
	if(pcap_setfilter(pcap, &fp) == -1) {
        fprintf(stderr, "Error setting filter\n");
		return { -1 };
    } 
    std::vector<uint8_t> buffer;
    bufferresize(1600);
    //可能会调用多次
    pcap_loop(pacp, 1, [](u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
 		memcpy(user, bytes, h->caplen)
	}, buffer.data());
	return { 0, std::move(buffer) };
}
