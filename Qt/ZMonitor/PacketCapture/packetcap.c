#include "packetcap.h"
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netinet/if_ether.h>

void printBandwidth(){
    system("clear");
    if (Lbandwidth == NULL) return;
    struct bandwidth* temp = Lbandwidth->node;
    while(temp){
        struct in_addr source, dest;
        source.s_addr = temp->source; dest.s_addr = temp->dest;
        char *s = inet_ntoa(source);
        printf("Source: %s\t \n",s);
        char *d = inet_ntoa(dest);
        printf("Dest: %s\t, Data revc(bytes): %d \n",d,temp->data * sizeof(char));
        temp = temp->next;
    }
    return;
}

struct bandwidth* lookupBandwidth(uint32_t source, uint32_t dest, uint16_t sport, uint16_t dport){
    struct bandwidth* temp;
    if (Lbandwidth)
        temp = Lbandwidth->node;
    else return NULL;
    while(temp){
        if (temp->source == source && temp->dest == dest && temp->dport == dport && temp->sport == sport)
            return temp;
        else temp = temp->next;
    }
    return NULL;
}

int addBandwidth(uint32_t source, uint32_t dest,uint16_t sport,uint16_t dport, uint32_t data){
    struct bandwidth* temp;
    if ((temp = lookupBandwidth(source,dest,sport,dport)) != NULL) temp->data += data;
    else {
        struct bandwidth* node =(struct bandwidth*) malloc(sizeof(struct bandwidth));
        node->data = data; node->dest = dest; node->source = source;node->dport = dport; node->sport = sport; node->next = NULL;
        if (Lbandwidth == NULL){
            Lbandwidth = (struct Listbandwidth*) malloc(sizeof(struct Listbandwidth));
            Lbandwidth->node = node;
        }
        else {
            temp = Lbandwidth->node;
            Lbandwidth->node = node;
            node->next = temp;
        }
    }
    return 1;
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){


    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_tcp *tcp;            /* The TCP header */
    const struct sniff_udp *udp;            /* The UDP header */

    int size_ip;
    int size_tcp;
    int size_udp;
    int size_payload;

    ethernet = (struct sniff_ethernet*)(packet);
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip) *4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    switch(ip->ip_p) {
        case IPPROTO_TCP:

            tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
            size_tcp = TH_OFF(tcp)*4;
            if (size_tcp < 20) {
                printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
                return;
            }
            size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

            /*printf("   Protocol: TCP\n");
            printf("       From: %s\n", inet_ntoa(ip->ip_src));
            printf("   Src port: %d\n", ntohs(tcp->th_sport));
            printf("         To: %s\n", inet_ntoa(ip->ip_dst));
            printf("   Dst port: %d\n", ntohs(tcp->th_dport));
            printf("  Data size: %d\n\n", size_payload);*/
            addBandwidth(ip->ip_src.s_addr,ip->ip_dst.s_addr,tcp->th_sport,tcp->th_dport,size_payload);
            //struct bandwidth* temptcp = lookupBandwidth(ip->ip_src.s_addr,ip->ip_dst.s_addr,tcp->th_sport,tcp->th_dport);
            //printf(" %d\n",temptcp->data);
            printBandwidth();

            break;
        case IPPROTO_UDP:

            udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
            size_udp = 8;
            size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);

            /*printf("   Protocol: UDP\n");
            printf("       From: %s\n", inet_ntoa(ip->ip_src));
            printf("   Src port: %d\n", ntohs(udp->uh_sport));
            printf("         To: %s\n", inet_ntoa(ip->ip_dst));
            printf("   Dst port: %d\n", ntohs(udp->uh_dport));
            printf("  Data size: %d\n\n", size_payload);*/
            addBandwidth(ip->ip_src.s_addr,ip->ip_dst.s_addr,udp->uh_sport,udp->uh_dport,size_payload);
            //struct bandwidth* tempudp = lookupBandwidth(ip->ip_src.s_addr,ip->ip_dst.s_addr,udp->uh_sport,udp->uh_dport);
            //printf(" %d\n",tempudp->data);

            printBandwidth();
            break;
        case IPPROTO_ICMP:
            //printf("   Protocol: ICMP\n");
            return;
        case IPPROTO_IP:
            //printf("   Protocol: IP\n");
            return;
        default:
            //printf("   Protocol: unknown\n");
            return;
    }
    return;
}

int getPacket()
{
    pcap_t *handle;		/* Session handle */
    char *dev = NULL;		/* Device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */

    struct bpf_program fp;		/* The compiled filter expression */
    char filter_exp[] = "ip";	/* The filter expression */
    bpf_u_int32 mask;		/* The netmask of our sniffing device */
    bpf_u_int32 net;		/* The IP of our sniffing device */
    int num_packets = -1;   /* number of packets we capture */

    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        fprintf(stderr, "Couldn't find default device: %s\n",
            errbuf);
        exit(EXIT_FAILURE);
    }

    /* get network number and mask associated with capture device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
            dev, errbuf);
        net = 0;
        mask = 0;
    }

    /* print capture info */
    printf("Device: %s\n", dev);
    printf("Number of packets: %d\n", num_packets);
    printf("Filter expression: %s\n", filter_exp);

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(EXIT_FAILURE);
    }

    /* make sure we're capturing on an Ethernet device [2] */
    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        exit(EXIT_FAILURE);
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* set callback function */
    pcap_loop(handle, num_packets, got_packet, NULL);

    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);

    printf("\nCapture complete.\n");
    return 0;
}

