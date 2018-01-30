#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <libfdisk/libfdisk.h>

#define _(x) x
#define DEV_PATH "/dev"
#define SYS_NET_PATH "/sys/class/net"
#define STATISTIC "/statistics"
#define RX_BYTES "/rx_bytes"
#define TX_BYTES "/tx_bytes"
#define SYS_X_STATISTIC(x,y) SYS_NET_PATH x STATISTIC y

#define MAX_INTERFACE 5
#define MAX_NAME 255

typedef struct interface_info{
    char name[MAX_NAME];
    unsigned char MAC[6];

    struct in_addr ip,broadcast,netmask;

    unsigned long rx_bytes,
        tx_bytes;
    unsigned rx_packets,
        tx_packets;
} interface_info ;

interface_info device[MAX_INTERFACE];

void print_info_device(){
    for (int i  = 0; i < MAX_INTERFACE; i++){
        if (device[i].name[0] == '\0')
            continue;
        printf("Interface name: %s\n",device[i].name);
    }
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

void getInterfacename(){
    char path[13];
    char buffer[8192];
    sprintf(path,"/proc/net/dev");
    FILE *procinfo = proc_fopen(path);
    if (procinfo == NULL)
        return;
    int count = 0;
    char name[8192];

    while (fgets(buffer, sizeof(buffer), procinfo)){
        count ++;
        if (count < 3)
            continue;
        sscanf(buffer,"%s",name);
        //if (strcmp(name, "lo:\0") == 0)
        //    continue;
        int i = 0;
        while(name[i]){
            if(name[i]==':')
                name[i]='\0';
            i++;
        }

        int fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
        struct ifreq ifr;

        strncpy(ifr.ifr_ifrn.ifrn_name, name,IFNAMSIZ-1);
        strncpy(device[count-3].name, name,IFNAMSIZ-1);
        ifr.ifr_ifru.ifru_addr.sa_family = AF_INET;
        ioctl(fd, SIOCGIFFLAGS, &ifr);
        ioctl(fd, SIOCGIFHWADDR, &ifr);
        unsigned char MAC[6];
        for (int i = 0; i < 6; i++){
            MAC[i] = ifr.ifr_ifru.ifru_hwaddr.sa_data[i];
            device[count-3].MAC[i] = MAC[i];
        }


        ioctl(fd, SIOCGIFADDR, &ifr);
        device[count-3].ip = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
        ioctl(fd, SIOCGIFBRDADDR, &ifr);
        device[count-3].broadcast = ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr;
        ioctl(fd, SIOCGIFNETMASK, &ifr);
        device[count-3].netmask = ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr;
        close(fd);
    }
    close(procinfo);

    print_info_device();

}

