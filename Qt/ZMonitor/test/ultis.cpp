#include "ultis.h"
#include <string.h>
#include <stdio.h>

using namespace std;


#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6


/* Ethernet header */
struct sniff_ethernet {
    u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
    u_short ether_type; /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
    u_char ip_vhl;		/* version << 4 | header length >> 2 */
    u_char ip_tos;		/* type of service */
    u_short ip_len;		/* total length */
    u_short ip_id;		/* identification */
    u_short ip_off;		/* fragment offset field */
    #define IP_RF 0x8000		/* reserved fragment flag */
    #define IP_DF 0x4000		/* dont fragment flag */
    #define IP_MF 0x2000		/* more fragments flag */
    #define IP_OFFMASK 0x1fff	/* mask for fragmenting bits */
    u_char ip_ttl;		/* time to live */
    u_char ip_p;		/* protocol */
    u_short ip_sum;		/* checksum */
    struct in_addr ip_src,ip_dst; /* source and dest address */
};
#define IP_HL(ip)		(((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)		(((ip)->ip_vhl) >> 4)
/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
    u_short th_sport;	/* source port */
    u_short th_dport;	/* destination port */
    tcp_seq th_seq;		/* sequence number */
    tcp_seq th_ack;		/* acknowledgement number */
    u_char th_offx2;	/* data offset, rsvd */
    #define TH_OFF(th)	(((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
    #define TH_FIN 0x01
    #define TH_SYN 0x02
    #define TH_RST 0x04
    #define TH_PUSH 0x08
    #define TH_ACK 0x10
    #define TH_URG 0x20
    #define TH_ECE 0x40
    #define TH_CWR 0x80
    #define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;		/* window */
    u_short th_sum;		/* checksum */
    u_short th_urp;		/* urgent pointer */
};

struct sniff_udp {
    uint16_t	uh_sport;		/* source port */
    uint16_t	uh_dport;		/* destination port */
    uint16_t	uh_ulen;		/* udp length */
    uint16_t	uh_sum;			/* udp checksum */
};

Interfaces::Interfaces()
{
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */

    if (pcap_findalldevs(&this->alldev, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }
}

void Interfaces::filt_ethernet()
{
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];

    for(pcap_if_t* d=this->alldev; d; d=d->next){

        handle = pcap_open_live(d->name,SNAP_LEN,1,1000, errbuf);
        if (handle == NULL) {
            continue;
        }

        if (pcap_datalink(handle) != DLT_EN10MB) {
            pcap_close(handle);
            continue;
        }

        this->ethernet.push_back(d->name);
    }
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    /* declare pointers to packet headers */
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_tcp *tcp;            /* The TCP header */
    const struct sniff_udp *udp;            /* The UDP header */
    const char *payload;                    /* Packet payload */

    int size_ip;
    int size_tcp;
    int size_udp;
    int size_payload;

    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);
    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    /* print source and destination IP addresses */
    printf("       From: %s\n", inet_ntoa(ip->ip_src));
    printf("         To: %s\n", inet_ntoa(ip->ip_dst));
}



Capturer::Capturer(std::__cxx11::string name)
{
    this->interface = name;
}

void Capturer::setInterface(std::__cxx11::string name)
{
    this->interface = name;
}

void Capturer::capture()
{
    bpf_u_int32 mask;		/* The netmask of our sniffing device */
    bpf_u_int32 net;		/* The IP of our sniffing device */
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    char filter_exp[] = "ip";	/* The filter expression */
    struct bpf_program fp;		/* The compiled filter expression */
    char dev[1024];
    pcap_t *handle;

    strncpy(dev,this->interface.c_str(),sizeof(dev));
    dev[sizeof(dev)-1] = 0;
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
            dev, errbuf);
        net = 0;
        mask = 0;
    }

    handle = pcap_open_live(dev,SNAP_LEN,1,1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        goto ERROR_LABEL;
    }

    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        goto ERROR_LABEL;
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        goto ERROR_LABEL;
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        goto ERROR_LABEL;
    }

    pcap_loop(handle, -1, got_packet, NULL);
    pcap_freecode(&fp);
ERROR_LABEL:
    pcap_close(handle);
    return;
}
