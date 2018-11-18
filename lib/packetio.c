//#include "packetio.h"
#include "device.h"
#include "packetio.h"

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
int sendFrame(const void* buf, int len, int ethtype, const void* destmac, int id){
	//ethernet II  packing 
	size_t size = sizeof(struct ether_header)+len+4;
	if (size < 64) size = 64;
	void* eth =  malloc(size);
	memset(eth, 0, size);
	struct ether_header * eth_hdr = (struct ether_header*) eth;
	struct Dev* device = getDevice(id);

	memcpy(eth_hdr->ether_dhost, destmac, ETH_ALEN);
	memcpy(eth_hdr->ether_shost, &device->mac, ETH_ALEN); 
	eth_hdr->ether_type = ethtype;
	memcpy(eth + sizeof(struct ether_header), buf, len); 

	//checksum needed
	char pcap_errbuf[PCAP_ERRBUF_SIZE];
	pcap_errbuf[0]='\0';
	
	pcap_t* pcap=pcap_open_live(device->name,96,0,0,pcap_errbuf);
	if (pcap_errbuf[0]!='\0') 
		fprintf(stderr,"%s",pcap_errbuf);
	if (!pcap) {
		free(eth);
		return -1;
	}
	if (pcap_inject(pcap, &eth, sizeof(eth))==-1) {
		pcap_perror(pcap,0);
		pcap_close(pcap);
		free(eth)
		return -1;
	}
	pcap_close(pcap);
	free(eth);
	return 0;
}

/**
 * @brief Register a callback function to be called each time an Ethernet II 
 * frame was received.
 *
 * @param callback The callback function.
 * @return 0 on success, -1 on error.
 * @see frameReceiveCallback
 */
int setFarmeReceiveCallback(frameReceiveCallback callback) { 
	char pcap_errbuf[PCAP_ERRBUF_SIZE];
	pcap_errbuf[0]='\0';
	
	pcap_t* pcap=pcap_open_live(if_name,96,0,0,pcap_errbuf);
	if (pcap_errbuf[0]!='\0') 
		fprintf(stderr,"%s",pcap_errbuf);
	if (!pcap) return -1;

	if(pcap_compile(descr, &fp, argv[1], 0, netp) == -1) {
        fprintf(stderr, "Error calling pcap_compile\n");
		return -1;
    } 
	
	if(pcap_setfilter(pcap, &fp) == -1) {
        fprintf(stderr, "Error setting filter\n");
		return -1;
    } 
	pcap_loop(pcap, -1, callback, NULL);
	return 0;
}
int getFrame(char *filter, char *back){
	

}
