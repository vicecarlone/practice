#include "zsocket.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <time.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>


void oom()
{
    fprintf(stderr, "out of virtual memory\n");
    exit(2);
}
void *xmalloc(size_t sz)
{
    void *p = calloc(sz, 1);
    if (!p) oom();
    return p;
}

char *xstrdup(const char *s)
{
    char *d = strdup(s);
    if (!d)
        oom();
    return d;
}
void *xrealloc(void *oldp, size_t sz)
{
    void *p = realloc(oldp, sz);
    if (!p)
    oom();
    return p;
}


char *safe_strncpy(char *dst, const char *src, size_t size)
{
    dst[size-1] = '\0';
    return strncpy(dst,src,size-1);
}


FILE *proc_fopen(const char *name)
{
    static char *buffer;
    static size_t pagesz;
    FILE *fd = fopen(name, "r");

    if (fd == NULL)
      return NULL;

    if (!buffer) {
      pagesz = getpagesize();
      buffer = (char*) (malloc(pagesz));
    }

    setvbuf(fd, buffer, _IOFBF, pagesz);
    return fd;
}


void parameterParser(int argc, char *argv[])
{
    return;
}
int extract_type_1_socket_inode(const char lname[], unsigned long * inode_p) {

    /* If lname is of the form "socket:[12345]", extract the "12345"
       as *inode_p.  Otherwise, return -1 as *inode_p.
       */

    if (strlen(lname) < PRG_SOCKET_PFXl+3) return(-1);

    if (memcmp(lname, PRG_SOCKET_PFX, PRG_SOCKET_PFXl)) return(-1);
    if (lname[strlen(lname)-1] != ']') return(-1);

    {
        char inode_str[strlen(lname + 1)];  /* e.g. "12345" */
        const int inode_str_len = strlen(lname) - PRG_SOCKET_PFXl - 1;
        char *serr;

        strncpy(inode_str, lname+PRG_SOCKET_PFXl, inode_str_len);
        inode_str[inode_str_len] = '\0';
        *inode_p = strtoul(inode_str, &serr, 0);
        if (!serr || *serr || *inode_p == ~0)
            return(-1);
    }
    return(0);
}
int extract_type_2_socket_inode(const char lname[], unsigned long * inode_p) {

    /* If lname is of the form "[0000]:12345", extract the "12345"
       as *inode_p.  Otherwise, return -1 as *inode_p.
       */

    if (strlen(lname) < PRG_SOCKET_PFX2l+1) return(-1);
    if (memcmp(lname, PRG_SOCKET_PFX2, PRG_SOCKET_PFX2l)) return(-1);

    {
        char *serr;

        *inode_p = strtoul(lname + PRG_SOCKET_PFX2l, &serr, 0);
        if (!serr || *serr || *inode_p == ~0)
            return(-1);
    }
    return(0);
}
void prg_cache_add(unsigned long inode, char *name, const char *scon)
{
    unsigned hi = PRG_HASHIT(inode);
    struct prg_node **pnp,*pn;

    prg_cache_loaded = 2;
    for (pnp = prg_hash + hi; (pn = *pnp); pnp = &pn->next) {
    if (pn->inode == inode) {
        /* Some warning should be appropriate here
           as we got multiple processes for one i-node */
        return;
    }
    }
    if (!(*pnp = malloc(sizeof(**pnp))))
    return;
    pn = *pnp;
    pn->next = NULL;
    pn->inode = inode;
    safe_strncpy(pn->name, name, sizeof(pn->name));

    {
    int len = (strlen(scon) - sizeof(pn->scon)) + 1;
    if (len > 0)
            safe_strncpy(pn->scon, &scon[len + 1], sizeof(pn->scon));
    else
            safe_strncpy(pn->scon, scon, sizeof(pn->scon));
    }

}
void prg_cache_load(){
    char line[LINE_MAX], eacces=0;
    int procfdlen, fd, cmdllen, lnamelen;
    char lname[30], cmdlbuf[512], finbuf[PROGNAME_WIDTH];
    unsigned long inode;
    const char *cs, *cmdlp;
    DIR *dirproc = NULL, *dirfd = NULL;
    struct dirent *direproc, *direfd;

    if (prg_cache_loaded) return;
    prg_cache_loaded = 1;
    cmdlbuf[sizeof(cmdlbuf) - 1] = '\0';
    if (!(dirproc=opendir(PATH_PROC))) goto fail;
    while (errno = 0, direproc = readdir(dirproc)) {
        for (cs = direproc->d_name; *cs; cs++)
            if (!isdigit(*cs))
            break;
        if (*cs)
            continue;
        procfdlen = snprintf(line,sizeof(line),PATH_PROC_X_FD,direproc->d_name);
        if (procfdlen <= 0 || procfdlen >= sizeof(line) - 5)
            continue;
        errno = 0;
        dirfd = opendir(line);
        if (! dirfd) {
            if (errno == EACCES)
            eacces = 1;
            continue;
        }
        line[procfdlen] = '/';
        cmdlp = NULL;
        while ((direfd = readdir(dirfd))) {
               /* Skip . and .. */
               if (!isdigit(direfd->d_name[0]))
                   continue;
            if (procfdlen + 1 + strlen(direfd->d_name) + 1 > sizeof(line))
                continue;
            memcpy(line + procfdlen - PATH_FD_SUFFl, PATH_FD_SUFF "/", PATH_FD_SUFFl + 1);
            safe_strncpy(line + procfdlen + 1, direfd->d_name,sizeof(line) - procfdlen - 1);
            lnamelen = readlink(line, lname, sizeof(lname) - 1);
            if (lnamelen == -1)
                continue;
            lname[lnamelen] = '\0';  /*make it a null-terminated string*/

            if (extract_type_1_socket_inode(lname, &inode) < 0)
                if (extract_type_2_socket_inode(lname, &inode) < 0)
                    continue;

            if (!cmdlp) {
                if (procfdlen - PATH_FD_SUFFl + PATH_CMDLINEl >=sizeof(line) - 5)
                    continue;
                safe_strncpy(line + procfdlen - PATH_FD_SUFFl, PATH_CMDLINE,
                                sizeof(line) - procfdlen + PATH_FD_SUFFl);
                fd = open(line, O_RDONLY);
                if (fd < 0)
                    continue;
                cmdllen = read(fd, cmdlbuf, sizeof(cmdlbuf) - 1);
                if (close(fd))
                    continue;
                if (cmdllen == -1)
                    continue;
                if (cmdllen < sizeof(cmdlbuf) - 1)
                    cmdlbuf[cmdllen]='\0';
                if (cmdlbuf[0] == '/' && (cmdlp = strrchr(cmdlbuf, '/')))
                    cmdlp++;
                else
                    cmdlp = cmdlbuf;
            }

            snprintf(finbuf, sizeof(finbuf), "%s/%s", direproc->d_name, cmdlp);

            prg_cache_add(inode, finbuf, "-");
        }
        closedir(dirfd);
        dirfd = NULL;
    }
    if (dirproc)
    closedir(dirproc);
    if (dirfd)
    closedir(dirfd);
    if (!eacces)
    return;
    if (prg_cache_loaded == 1) {
    fail:
    fprintf(stderr,_("(No info could be read for \"-p\": geteuid()=%d but you should be root.)\n"),
        geteuid());
    }
    else
    fprintf(stderr, _("(Not all processes could be identified, non-owned process info\n"
             " will not be shown, you would have to be root to see it all.)\n"));
}




const char *prg_cache_get(unsigned long inode)
{
    unsigned hi = PRG_HASHIT(inode);
    struct prg_node *pn;

    for (pn = prg_hash[hi]; pn; pn = pn->next)
    if (pn->inode == inode)
        return (pn->name);
    return ("-");
}

char digitParser(int x){
    switch (x) {
    case 0:
        return '0';
        break;
    case 1:
        return '1';
        break;
    case 2:
        return '2';
        break;
    case 3:
        return '3';
        break;
    case 4:
        return '4';
        break;
    case 5:
        return '5';
        break;
    case 6:
        return '6';
        break;
    case 7:
        return '7';
        break;
    case 8:
        return '8';
        break;
    case 9:
        return '9';
        break;
    default:
        return '\0';
    }
}
void addressParser(char *addr, char* output){
    int temp = 0;
    for (int i = 0; i < 8; i++){
        if ( addr[i]==  '0')temp =16 * temp + 0;
        else if  (addr[i] =='1')temp =16 * temp + 1;
        else if  (addr[i] =='2')temp =16 * temp + 2;
        else if  (addr[i] =='3')temp =16 * temp + 3;
        else if  (addr[i] =='4')temp =16 * temp + 4;
        else if  (addr[i] =='5')temp =16 * temp + 5;
        else if  (addr[i] =='6')temp =16 * temp + 6;
        else if  (addr[i] =='7')temp =16 * temp + 7;
        else if  (addr[i] =='8')temp =16 * temp + 8;
        else if  (addr[i] =='9')temp =16 * temp + 9;
        else if  (addr[i] =='A')temp =16 * temp + 10;
        else if  (addr[i] =='B')temp =16 * temp + 11;
        else if  (addr[i] =='C')temp =16 * temp + 12;
        else if  (addr[i] =='D')temp =16 * temp + 13;
        else if  (addr[i] =='E')temp =16 * temp + 14;
        else if  (addr[i] =='F')temp =16 * temp + 15;

        if(i%2) {
            if (temp > 99) output[12 - i/2*4] = digitParser(temp/100);
                    else output[12 - i/2*4] = '0';
            if (temp > 9) output[13 - i/2*4] = digitParser((temp/10)%10);
                    else output[13 - i/2*4] = '0';
            output[14 - i/2*4] = digitParser((temp%100) %10);
            if (i != 7) output[11 - i/2*4] = '.';
            temp = 0;
        }


    }
}


float findBandwidth(char* rem_adrr, char* local_addr, int rem_port, int local_port){
    uint32_t source = 0, dest = 0, reS = 0, reD =0;
    for (int i = 0; i < 8; i++){
        if (rem_adrr[i] == '0') dest = dest*16;
        else if (rem_adrr[i] == '1') dest = dest *16 + 1;
        else if (rem_adrr[i] == '2') dest = dest *16 + 2;
        else if (rem_adrr[i] == '3') dest = dest *16 + 3;
        else if (rem_adrr[i] == '4') dest = dest *16 + 4;
        else if (rem_adrr[i] == '5') dest = dest *16 + 5;
        else if (rem_adrr[i] == '6') dest = dest *16 + 6;
        else if (rem_adrr[i] == '7') dest = dest *16 + 7;
        else if (rem_adrr[i] == '8') dest = dest *16 + 8;
        else if (rem_adrr[i] == '9') dest = dest *16 + 9;
        else if (rem_adrr[i] == 'A') dest = dest *16 + 10;
        else if (rem_adrr[i] == 'B') dest = dest *16 + 11;
        else if (rem_adrr[i] == 'C') dest = dest *16 + 12;
        else if (rem_adrr[i] == 'D') dest = dest *16 + 13;
        else if (rem_adrr[i] == 'E') dest = dest *16 + 14;
        else if (rem_adrr[i] == 'F') dest = dest *16 + 15;

        if (local_addr[i] == '0') source = source *16;
        else if (local_addr[i] == '1') source = source *16 + 1;
        else if (local_addr[i] == '2') source = source *16 + 2;
        else if (local_addr[i] == '3') source = source *16 + 3;
        else if (local_addr[i] == '4') source = source *16 + 4;
        else if (local_addr[i] == '5') source = source *16 + 5;
        else if (local_addr[i] == '6') source = source *16 + 6;
        else if (local_addr[i] == '7') source = source *16 + 7;
        else if (local_addr[i] == '8') source = source *16 + 8;
        else if (local_addr[i] == '9') source = source *16 + 9;
        else if (local_addr[i] == 'A') source = source *16 + 10;
        else if (local_addr[i] == 'B') source = source *16 + 11;
        else if (local_addr[i] == 'C') source = source *16 + 12;
        else if (local_addr[i] == 'D') source = source *16 + 13;
        else if (local_addr[i] == 'E') source = source *16 + 14;
        else if (local_addr[i] == 'F') source = source *16 + 15;
        if (i%2){
            reS += source* (i == 7 ? 16*16*16 : (i == 5) ? 16*16 : (i == 3) ? 16 : 1);
            reD += dest* (i == 7 ? 16*16*16 : (i == 5) ? 16*16 : (i == 3) ? 16 : 1);
            dest = 0;
            source =0;
        }
    }
    struct bandwidth* temp = lookupBandwidth(reS,reD,(uint16_t)local_port,(uint16_t)rem_port);
    if (temp) return temp->data;
    else return 0.0f;
}

int readSocket(unsigned int kind)
{
    prg_cache_load();
    switch (kind) {
    case 0:
        readSocket(1);
        readSocket(2);
        break;
    case 1:
        procinfo = proc_fopen(_PATH_PROCNET_TCP);
        break;
    case 2:
        procinfo = proc_fopen(_PATH_PROCNET_UDP);
        break;
    default:
        break;
    }

    char buffer[8192];
    static int count = 0;
    if (procinfo == NULL){
        return -1;
    }
    else {
        do {
            if (fgets(buffer, sizeof(buffer), procinfo)){
                unsigned long rxq, txq, time_len, retr, inode;
                int num, local_port, rem_port, d, state, uid, timer_run, timeout;
                char rem_addr[128], local_addr[128];\
                num = sscanf(buffer,
                "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
                char loc[16]; loc[0] = '\0';char rem[16]; rem[0] = '\0';
                if (count){
                    addressParser(&local_addr,&loc);
                    addressParser(&rem_addr,&rem);
                    printf("%s:%d \t %s:%d \t%ld \t %s \t%s \t \t \t %s \t %.1f\n",loc,local_port,rem,rem_port,inode,tcp_state[state],protocol_type[kind],prg_cache_get(inode),findBandwidth(&rem_addr,&local_addr,rem_port,local_port));
                }
                else {
                    printf("Local address: \t\t Dest address: \t \tInode: \t State: \tProtocol: \t Process:\n");
                    count++;
                }

            }
        } while (!feof(procinfo));
        fclose(procinfo);
    }
    return 0;
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
    system("clear");
    static int count = 0;
    count++;
    printf("%d \n",count);

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
/*
            tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
            size_tcp = TH_OFF(tcp)*4;
            if (size_tcp < 20) {
                printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
                return;
            }
            size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

            printf("   Protocol: TCP\n");
            printf("       From: %s\n", inet_ntoa(ip->ip_src));
            printf("   Src port: %d\n", ntohs(tcp->th_sport));
            printf("         To: %s\n", inet_ntoa(ip->ip_dst));
            printf("   Dst port: %d\n", ntohs(tcp->th_dport));
            printf("  Data size: %d\n\n", size_payload);
            addBandwidth(ip->ip_src.s_addr,ip->ip_dst.s_addr,tcp->th_sport,tcp->th_dport,size_payload);
            struct bandwidth *size = lookupBandwidth(ip->ip_src.s_addr,ip->ip_dst.s_addr,tcp->th_sport,tcp->th_dport);
            if (size != NULL)
                printf("%d \n",size->data);*/
            break;

        case IPPROTO_UDP:
            return;
            /*
            udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
            size_udp = 8;
            size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);

            printf("   Protocol: UDP\n");
            printf("       From: %s\n", inet_ntoa(ip->ip_src));
            printf("   Src port: %d\n", ntohs(udp->uh_sport));
            printf("         To: %s\n", inet_ntoa(ip->ip_dst));
            printf("   Dst port: %d\n", ntohs(udp->uh_dport));
            printf("  Data size: %d\n\n", size_payload);*/

            break;
        case IPPROTO_ICMP:
            printf("   Protocol: ICMP\n");
            return;
        case IPPROTO_IP:
            printf("   Protocol: IP\n");
            return;
        default:
            printf("   Protocol: unknown\n");
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

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, -1, errbuf);
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
