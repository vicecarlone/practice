#include <sniffer.h>
#include <utils.h>

void * wait_queue;
void * connected_queue;
static pthread_mutex_t print_mutex;

void * print_state(void * param){
    while(1){
        usleep(REFRESH_OUTPUT);
        system("clear");
        printf("waiting: \n");
        check_timeout(wait_queue);
        print_queue(wait_queue);
        printf("connected: \n");
        print_queue(connected_queue);
    }
}


void * filt_inferface (void * param){
    char *name = (char*)param;
    if (strcmp(name,"lo")==0)
        return;
    pcap_t *handle;
    struct bpf_program fp;		/* The compiled filter expression */
    char filter_exp[] = "ip";	/* The filter expression */
    bpf_u_int32 mask;		/* The netmask of our sniffing device */
    bpf_u_int32 net;		/* The IP of our sniffing device */
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */

    if (pcap_lookupnet(name, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
            name, errbuf);
        net = 0;
        mask = 0;
    }

    handle = pcap_open_live(name,SNAP_LEN,1,1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", name, errbuf);
        return;
    }

    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", name);
        return;
    }


    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        return;
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
        return;
    }

    printf("start capturing on interface:%s\n",name);

    pcap_loop(handle, -1, got_packet, NULL);

    pcap_freecode(&fp);
    pcap_close(handle);
}


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{


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
    //printf("       From: %s\n", inet_ntoa(ip->ip_src));
    //printf("         To: %s\n", inet_ntoa(ip->ip_dst));

    /* determine protocol */
    switch(ip->ip_p) {
        case IPPROTO_TCP:
            //printf("   Protocol: TCP\n");
            /* define/compute tcp header offset */
            tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
            size_tcp = TH_OFF(tcp)*4;
            if (size_tcp < 20) {
                printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
                return;
            }

            /* define/compute tcp payload (segment) offset */
            payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);

            /* compute tcp payload (segment) size */
            size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

            switch (tcp->th_flags){
            case 0x02: //SYN
                element_push(wait_queue,element_create(ip->ip_src,ip->ip_dst,tcp->th_sport,tcp->th_dport));
                break;
            case 0x12: //SYS,ACK
                element_remove(wait_queue,ip->ip_dst,ip->ip_src,tcp->th_dport,tcp->th_sport);
                element_push(connected_queue,element_create(ip->ip_src,ip->ip_dst,tcp->th_sport,tcp->th_dport));
                break;
            }

            if (tcp->th_flags & 0x01 || (tcp->th_flags >> 2) & 0x01){
                element_remove(connected_queue,ip->ip_src,ip->ip_dst,tcp->th_sport,tcp->th_dport);
                element_remove(connected_queue,ip->ip_dst,ip->ip_src,tcp->th_dport,tcp->th_sport);
                //element_remove(wait_queue,ip->ip_src,ip->ip_dst,tcp->th_sport,tcp->th_dport);
                //element_remove(wait_queue,ip->ip_dst,ip->ip_src,tcp->th_dport,tcp->th_sport);
            }



            break;

//            case IPPROTO_UDP:
//            //printf("   Protocol: UDP\n");
//            /* define udp header */
//            udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
//            size_udp = 8;

//            //printf("   Src port: %d\n", ntohs(udp->uh_sport));
//            //printf("   Dst port: %d\n", ntohs(udp->uh_dport));

//            /* define/compute tcp payload (segment) offset */
//            payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_udp);

//            /* compute tcp payload (segment) size */
//            size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);

//            break;
//        case IPPROTO_ICMP:

//            printf("   Protocol: ICMP\n");
//            break;
//            return;
//        case IPPROTO_IP:

//            printf("   Protocol: IP\n");
//            break;
//            return;

        default:
            break;
    }

return;
}


int main(int argc, char *argv[])
{

    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */

    wait_queue = vector_create();
    connected_queue = vector_create();
    pcap_if_t *alldevs,*d;
    int num_threads = 0;

    pthread_mutex_init(&print_mutex,NULL);


    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }

    for(d=alldevs; d; d=d->next)
    {
        num_threads++;
        printf("%d. %s", num_threads, d->name);
        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" No description available for this device\n");
    }

    pthread_t *thread_pid = (pthread_t *)malloc(sizeof(pthread_t)*num_threads);

    int n =0;
    for(d = alldevs; d; d = d->next){

        if(pthread_create(&thread_pid[n],NULL,&filt_inferface,(void *)d->name))
            printf("error creating thread");
        n++;
    }

    pthread_t print_thread;
    pthread_create(&print_thread,NULL,&print_state,NULL);


    for (int i = 0; i < n; i++){
        pthread_join(thread_pid[i],NULL);
    }

    printf("\nCapture complete.\n");
    return 0;
}
