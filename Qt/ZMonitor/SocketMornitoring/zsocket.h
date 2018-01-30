#ifndef ZSOCKET_H
#define ZSOCKET_H


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <stddef.h>
#include <limits.h>
#include <sys/types.h>
#include <arpa/inet.h>



#define	LINE_MAX		_POSIX2_LINE_MAX
#define PROGNAME_WIDTH 20
#define SELINUX_WIDTH 50
#define PRG_HASH_SIZE 211

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6


static struct prg_node {
    struct prg_node *next;
    unsigned long inode;
    char name[PROGNAME_WIDTH];
    char scon[SELINUX_WIDTH];
} *prg_hash[PRG_HASH_SIZE];

struct bandwidth {
    uint32_t source;
    uint32_t dest;
    uint16_t sport;
    uint16_t dport;
    uint32_t data;
    struct bandwidth* next;
};

static struct Listbandwidth {
    struct bandwidth* node;
} * Lbandwidth = NULL;

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

static char prg_cache_loaded = 0;

#define PRG_HASHIT(x) ((x) % PRG_HASH_SIZE)

#define PATH_PROC "/proc"
#define PROC_STAT "/stat"
#define PROC_STATM "/statm"

#define _PATH_PROCNET_TCP   "/proc/net/tcp"
#define _PATH_PROCNET_UDP   "/proc/net/udp"

#define _(String) (String)
#define N_(String) (String)

#define PRG_LOCAL_ADDRESS "local_address"
#define PRG_INODE	 "inode"
#define PRG_SOCKET_PFX    "socket:["
#define PRG_SOCKET_PFXl (strlen(PRG_SOCKET_PFX))
#define PRG_SOCKET_PFX2   "[0000]:"
#define PRG_SOCKET_PFX2l  (strlen(PRG_SOCKET_PFX2))

#define PATH_FD_SUFF	"fd"
#define PATH_FD_SUFFl       strlen(PATH_FD_SUFF)
#define PATH_PROC_X_FD      PATH_PROC "/%s/" PATH_FD_SUFF
#define PATH_CMDLINE	"cmdline"
#define PATH_CMDLINEl       strlen(PATH_CMDLINE)

static const char *tcp_state[] =
{
    "",
    N_("ESTABLISHED"),
    N_("SYN_SENT   "),
    N_("SYN_RECV   "),
    N_("FIN_WAIT1  "),
    N_("FIN_WAIT2  "),
    N_("TIME_WAIT  "),
    N_("CLOSE      "),
    N_("CLOSE_WAIT "),
    N_("LAST_ACK   "),
    N_("LISTEN     "),
    N_("CLOSING    ")
};

static const char *protocol_type[] =
{
    "",
    N_("TCP"),
    N_("UPD")
};

void *xmalloc(size_t sz);
void *xrealloc(void *p, size_t sz);
char *xstrdup(const char *src);

#define new(p) ((p) = xmalloc(sizeof(*(p))))


FILE *procinfo;

FILE *proc_fopen(const char *name);



int readSocket(unsigned int kind);

void parameterParser(int argc, char *argv[]);

int addBandwidth(uint32_t , uint32_t ,uint16_t ,uint16_t , uint32_t);

struct bandwidth* lookupBandwidth(uint32_t source, uint32_t dest, uint16_t sport, uint16_t dport);

int getPacket();

#endif // ZSOCKET_H
