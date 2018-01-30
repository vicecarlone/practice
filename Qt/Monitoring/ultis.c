#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>

#include "ultis.h"

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
void oom()
{
    fprintf(stderr, "out of virtual memory\n");
    exit(2);
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


int readProc(char* pid)
{
    char path[12];
    char buffer[8192];
    sprintf(path,PATH_PROC "/%s"PROC_STAT,pid);
    procinfo = proc_fopen(path);

    if (procinfo == NULL)
        return -1;

    int id, ppid, pgrp, session, tty_nr, tpgid, exit_signal, processor, exit_code;
    unsigned int flags, rt_priority, policy;
    long unsigned int minflt, cminflt, majflt, cmajflt, utime, stime, vsize, rsslim, startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap, guest_time, start_data, end_data, start_brk, arg_start, arg_end, env_start, env_end;
    long int cutime, cstime, priority, nice, num_threads, itrealvalue, rss, cguest_time;
    long long unsigned int starttime, delayacct_blkio_ticks;
    char* name;
    char state;

    if (fgets(buffer, sizeof(buffer), procinfo))
        sscanf(buffer,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
               &id,&name,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itrealvalue,&starttime,&vsize,&rss,&rsslim,&startcode,&endcode,&startstack,&kstkesp,&kstkeip,&signal,&blocked,&sigignore,&sigcatch,&wchan,&nswap,&cnswap,&exit_signal,&processor,&rt_priority,&policy,&delayacct_blkio_ticks,&guest_time,&cguest_time, &start_data, &end_data, &start_brk, &arg_start, &arg_end, &env_start, &env_end, &exit_code);
    fclose(procinfo);

    /*tty != 0 -> process else deamon*/

    printf("%d,%s,%c,%lu \n",id,&name,state,vsize);
    return 1;

}

void getProc(){
    const char *cs;
    DIR *dirproc = NULL;
    struct dirent *proc;
    if (!(dirproc=opendir(PATH_PROC))) return;

    while(errno = 0, proc = readdir(dirproc)){
        for (cs = proc->d_name; *cs; cs++)
            if (!isdigit(*cs))
            break;
        if (*cs)
            continue;
        readProc(proc->d_name);
    }


}

void parameterParser(int argc, char *argv[])
{
    return;
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
    }
}
void addressParser(char addr[128], char output[16]){
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
            if (temp > 99) output[0 + i/2*4] = digitParser(temp/100);
                    else output[0 + i/2*4] = '0';
            if (temp > 9) output[1 + i/2*4] = digitParser((temp/10)%10);
                    else output[1 + i/2*4] = '0';
            output[2 + i/2*4] = digitParser((temp%100) %10);
            if (i != 7) output[3 + i/2*4] = '.';
            temp = 0;
        }


    }
}


void readSocket(unsigned int kind)
{
    prg_cache_load();
    if (kind == 1)
        procinfo = proc_fopen(_PATH_PROCNET_TCP);
    else if (kind == 2)
        procinfo = proc_fopen(_PATH_PROCNET_UDP);
    char buffer[8192];
    int count = 0;
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
                    printf("%s:%d \t %s:%d \t%ld \t %s \t%s \t \t \t %s\n",loc,local_port,rem,rem_port,inode,tcp_state[state],protocol_type[kind],prg_cache_get(inode));
                }
                else {
                    printf("Local address: \t\t Dest address: \t \tInode: \t State: \tProtocol: \t Process:\n");
                }
                count++;
            }
        } while (!feof(procinfo));
        fclose(procinfo);
    }
}
