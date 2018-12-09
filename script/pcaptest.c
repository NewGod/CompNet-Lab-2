#include <stdio.h>
#include <pcap.h>
#include <pthread.h> 
#include <unistd.h>
#include <string.h>

bpf_u_int32 mask;		/* The netmask of our sniffing device */
bpf_u_int32 net;		/* The IP of our sniffing device */
char *dev;
void *myThreadFun(void *vargp) 
{ 
    sleep(1); 
    printf("Printing GeeksQuiz from Thread \n"); 
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL) {
	    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return NULL;
    }
    char filter_exp[] = "";	/* The filter expression */
	struct bpf_program fp;
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return NULL;
	}
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return NULL;
	}
	struct pcap_pkthdr header;	/* The header that pcap gives us */
	const u_char *packet;		/* The actual packet */
	/* Grab a packet */
	packet = pcap_next(handle, &header);
	/* Print its length */
	printf("Jacked a packet with length of [%d]\n", header.len);
    printf("packet: %s", packet);
	/* And close the session */
	pcap_close(handle);
    return NULL; 
} 

int main(int argc, char *argv[])
{

    char errbuf[PCAP_ERRBUF_SIZE];
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return 0;
    }
    printf("Device: %s\n", dev);
	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s\n", dev);
		net = 0;
		mask = 0;
	}

    pthread_t thread_id; 
    printf("Before Thread\n"); 
    pthread_create(&thread_id, NULL, myThreadFun, NULL); 

    pcap_t *handle;
	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL) {
	    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return 0;
    }
    const char *message = "Hello World\n";
    size_t size = strlen(message);
    
	if (pcap_inject(handle, &message, size)==-1) {
		pcap_perror(handle,0);
        return 0;
	}
	pcap_close(handle);
    
    pthread_join(thread_id, NULL); 
    printf("After Thread\n"); 
}
