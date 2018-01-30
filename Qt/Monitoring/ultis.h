#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <stddef.h>
#include <limits.h>



#define	LINE_MAX		_POSIX2_LINE_MAX
#define PROGNAME_WIDTH 20
#define SELINUX_WIDTH 50
#define PRG_HASH_SIZE 211

static struct prg_node {
    struct prg_node *next;
    unsigned long inode;
    char name[PROGNAME_WIDTH];
    char scon[SELINUX_WIDTH];
} *prg_hash[PRG_HASH_SIZE];

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

struct  nodePort{
    int port;
    int hostProc;
    int destProc;
    struct nodePort *next;
};
struct listPort{
    struct nodePort* root;
};



void readSocket(unsigned int kind);

void parameterParser(int argc, char *argv[]);

void getProc();

int readProc (char* pid);

