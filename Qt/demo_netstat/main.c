#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <paths.h>
#include <pwd.h>
#include <getopt.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <net/if.h>
#include <dirent.h>

#include "util.h"

FILE *procinfo;

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

#define PATH_PROC	   "/proc"
#define PATH_FD_SUFF	"fd"
#define PATH_FD_SUFFl       strlen(PATH_FD_SUFF)
#define PATH_PROC_X_FD      PATH_PROC "/%s/" PATH_FD_SUFF
#define PATH_CMDLINE	"cmdline"
#define PATH_CMDLINEl       strlen(PATH_CMDLINE)

#define PRG_HASH_SIZE 211

#define PROGNAME_WIDTH 20
#define SELINUX_WIDTH 50

#define FLAG_NUM_HOST  4
#define FLAG_NUM_PORT  8

int flag_prg = 1;
int flag_not = 0;
int flag_wide = 0;
int flag_opt = 0;
int flag_all = 0;


static const char *tcp_state[] =
{
    "",
    N_("ESTABLISHED"),
    N_("SYN_SENT"),
    N_("SYN_RECV"),
    N_("FIN_WAIT1"),
    N_("FIN_WAIT2"),
    N_("TIME_WAIT"),
    N_("CLOSE"),
    N_("CLOSE_WAIT"),
    N_("LAST_ACK"),
    N_("LISTEN"),
    N_("CLOSING")
};


static struct prg_node {
    struct prg_node *next;
    unsigned long inode;
    char name[PROGNAME_WIDTH];
    char scon[SELINUX_WIDTH];
} *prg_hash[PRG_HASH_SIZE];

static char prg_cache_loaded = 0;

#define PRG_HASHIT(x) ((x) % PRG_HASH_SIZE)

static void prg_cache_add(unsigned long inode, char *name, const char *scon)
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

static const char *prg_cache_get(unsigned long inode)
{
    unsigned hi = PRG_HASHIT(inode);
    struct prg_node *pn;

    for (pn = prg_hash[hi]; pn; pn = pn->next)
    if (pn->inode == inode)
        return (pn->name);
    return ("-");
}

static const char *prg_cache_get_con(unsigned long inode)
{
    unsigned hi = PRG_HASHIT(inode);
    struct prg_node *pn;

    for (pn = prg_hash[hi]; pn; pn = pn->next)
    if (pn->inode == inode)
        return (pn->scon);
    return ("-");
}

static void prg_cache_clear(void)
{
    struct prg_node **pnp,*pn;

    if (prg_cache_loaded == 2)
    for (pnp = prg_hash; pnp < prg_hash + PRG_HASH_SIZE; pnp++)
        while ((pn = *pnp)) {
        *pnp = pn->next;
        free(pn);
        }
    prg_cache_loaded = 0;
}

static void wait_continous(void)
{
    fflush(stdout);
    sleep(1);
}

static int extract_type_1_socket_inode(const char lname[], unsigned long * inode_p) {

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



static int extract_type_2_socket_inode(const char lname[], unsigned long * inode_p) {

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



static void prg_cache_load(void)
{
    char line[LINE_MAX], eacces=0;
    int procfdlen, fd, cmdllen, lnamelen;
    char lname[30], cmdlbuf[512], finbuf[PROGNAME_WIDTH];
    unsigned long inode;
    const char *cs, *cmdlp;
    DIR *dirproc = NULL, *dirfd = NULL;
    struct dirent *direproc, *direfd;

    if (prg_cache_loaded || !flag_prg) return;
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

FILE *proc_fopen(const char *name)
{
    static char *buffer;
    static size_t pagesz;
    FILE *fd = fopen(name, "r");

    if (fd == NULL)
      return NULL;

    if (!buffer) {
      pagesz = getpagesize();
      buffer = malloc(pagesz);
    }

    setvbuf(fd, buffer, _IOFBF, pagesz);
    return fd;
}



int main(int argc, char *argv[]){
    prg_cache_load();

    procinfo = proc_fopen(_PATH_PROCNET_TCP);
    char buffer[8192];					\
    if (procinfo == NULL){
        return -1;
    }
    else {
        int a = 0;
        do {
            if (fgets(buffer, sizeof(buffer), procinfo)){
                unsigned long rxq, txq, time_len, retr, inode;
                int num, local_port, rem_port, d, state, uid, timer_run, timeout;
                char rem_addr[128], local_addr[128], timers[64];\
                num = sscanf(buffer,
                "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
                printf("Local addr: %s:%d \t Inode: %ld \t Process: %s State: %d\n",local_addr,local_port,inode,prg_cache_get(inode),state);
            }
        } while (!feof(procinfo));
        fclose(procinfo);
    }


    //printf("%ld",strtol("AB",NULL,16));
    return 0;
}
