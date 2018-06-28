#include "proc.h"

#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <fstream>


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
Proc::Proc()
{
    //2998
    this->init();
}

Proc::Proc(unsigned int pid)
{
    this->setPid(pid);
    this->init();
}

void Proc::getCmdline()
{
    std::string path = string(PATH_PROC) + "/" + to_string(this->pid) + string(PATH_CMD);

    FILE * proc = proc_fopen(path.c_str());
    char buffer[1<<14];
    if (fgets(buffer,sizeof(buffer),proc)){
        string buff(buffer);
        string delimiter(" ");

        size_t pos = 0;
        std::string arg;
        while ((pos = buff.find(delimiter)) != std::string::npos) {
            arg = buff.substr(0, pos);
            this->cmdline.push_back(arg);
            buff.erase(0, pos + delimiter.length());
        }
        this->cmdline.push_back(buff);
    }
    fclose(proc);

}

void Proc::fetchInfoCpu()
{
    std::string _spid = std::string(PATH_PROC) +"/" +std::to_string(this->pid) + std::string(PATH_STAT);
    char buffer[1<<14];

    FILE* procinfo = proc_fopen(_spid.c_str());
    if (procinfo == NULL)
    {
        std::cout <<"Process not found"<<std::endl;
        return;
    }


    int id, ppid, pgrp, session, tty_nr, tpgid, exit_signal, processor, exit_code;
    unsigned int flags, rt_priority, policy;
    long unsigned int minflt, cminflt, majflt, cmajflt, utime, stime, vsize, rsslim, startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap, guest_time, start_data, end_data, start_brk, arg_start, arg_end, env_start, env_end;
    long int cutime, cstime, priority, nice, num_threads, itrealvalue, rss, cguest_time;
    long long unsigned int starttime, delayacct_blkio_ticks;
    char name[PATH_MAX];
    char state;
    struct timeval tv;

    if (fgets(buffer, sizeof(buffer), procinfo))
        sscanf(buffer,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
               &id,name,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itrealvalue,&starttime,&vsize,&rss,&rsslim,&startcode,&endcode,&startstack,&kstkesp,&kstkeip,&signal,&blocked,&sigignore,&sigcatch,&wchan,&nswap,&cnswap,&exit_signal,&processor,&rt_priority,&policy,&delayacct_blkio_ticks,&guest_time,&cguest_time, &start_data, &end_data, &start_brk, &arg_start, &arg_end, &env_start, &env_end, &exit_code);
    fclose(procinfo);

    gettimeofday(&tv,NULL);
    long long timenow = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    this->cpu_used = (float)(utime + stime + cstime + cutime - this->time.cstime - this->time.cutime - this->time.stime - this->time.utime) / (float)sysconf(_SC_CLK_TCK) /(timenow - this->time.last_time) * 100000.0f;
    this->time.cstime = cstime;
    this->time.cutime = cutime;
    this->time.stime = stime;
    this->time.utime = utime;
    gettimeofday(&tv,NULL);
    this->time.last_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void Proc::fetchInfoNetwork()
{
    std::string _fd = std::string(PATH_PROC) +"/" +std::to_string(this->pid) + std::string(PATH_FD);
    DIR *dir = NULL;
    struct dirent *dirfd;
    char lname[30];
    unsigned long inode;
    const char* pathdir = _fd.c_str();

    dir = opendir(pathdir);

    while((dirfd = readdir(dir))){
        if (!isdigit(dirfd->d_name[0]))
            continue;\

        std::string line(dirfd->d_name);
        line = _fd + "/" + line;


        int lnamelen = readlink(line.c_str(),lname,sizeof(lname));

        if(lnamelen == -1)
            continue;
        lname[lnamelen] = '\0';


        if (extractInodeType1(lname, &inode) < 0)
            if (extractInodeType2(lname, &inode) < 0)
                continue;
        this->inodes.push_back(inode);
    }
    closedir(dir);
    dirfd = NULL;


}

void Proc::fetchInfoMemory()
{
    FILE *procinfo;
    std::string path = std::string(PATH_PROC) + "/" + std::to_string(this->pid) + std::string(PATH_STATMEM);
    char buffer[8192];

    procinfo = proc_fopen(path.c_str());
    if (procinfo == NULL)
        return;

    if (fgets(buffer,sizeof(buffer), procinfo))
        sscanf(buffer,"%lu %lu %lu",&this->virtualMem,&this->resMem,&this->sharedMem);
    //this->sharedMem * sysconf(_SC_);
    fclose(procinfo);
}

int Proc::extractInodeType1(const char lname[], unsigned long *inode)
{
    if (strlen(lname) < PRG_SOCKET_PFXl+3) return(-1);

    if (memcmp(lname, PRG_SOCKET_PFX, PRG_SOCKET_PFXl)) return(-1);
    if (lname[strlen(lname)-1] != ']') return(-1);

    {
        char inode_str[strlen(lname + 1)];  /* e.g. "12345" */
        const int inode_str_len = strlen(lname) - PRG_SOCKET_PFXl - 1;
        char *serr;

        strncpy(inode_str, lname+PRG_SOCKET_PFXl, inode_str_len);
        inode_str[inode_str_len] = '\0';
        *inode = strtoul(inode_str, &serr, 0);
        if (!serr || *serr || *inode == ~0)
            return(-1);
    }
    return(0);
}

int Proc::extractInodeType2(const char lname[], unsigned long *inode)
{
    if (strlen(lname) < PRG_SOCKET_PFX2l+1) return(-1);
    if (memcmp(lname, PRG_SOCKET_PFX2, PRG_SOCKET_PFX2l)) return(-1);

    {
        char *serr;

        *inode = strtoul(lname + PRG_SOCKET_PFX2l, &serr, 0);
        if (!serr || *serr || *inode == ~0)
            return(-1);
    }
    return(0);
}



void Proc::scanTcp()
{
    FILE* procinfo = proc_fopen(_PATH_PROCNET_TCP);
    if (procinfo == NULL)
        return;

    char buffer[1<<14];
    unsigned long rxq, txq, time_len, retr, inode;
    int num, local_port, rem_port, d, state, uid, timer_run, timeout;
    char rem_addr[128], local_addr[128] ;
    do {
        if (fgets(buffer, sizeof(buffer), procinfo)){
            num = sscanf(buffer,
            "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                 &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                 &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);

            auto it = find(this->inodes.begin(),this->inodes.end(),inode);
            if (it != this->inodes.end()){
                struct Connection conn;
                inet_pton(AF_INET,local_addr,&conn.local_addr);
                inet_pton(AF_INET,rem_addr,&conn.remote_addr);
                conn.local_port = local_port;
                conn.remote_port = rem_port;
                nodetoprocess[inode] = conn;
            }
        }
    }
    while(!feof(procinfo));
    fclose(procinfo);
}

void Proc::scanUdp()
{
    FILE* procinfo = proc_fopen(_PATH_PROCNET_UDP);
    if (procinfo == NULL)
        return;

    char buffer[1<<14];
    unsigned long rxq, txq, time_len, retr, inode;
    int num, local_port, rem_port, d, state, uid, timer_run, timeout;
    char rem_addr[128], local_addr[128] ;
    do {
        if (fgets(buffer, sizeof(buffer), procinfo)){
            num = sscanf(buffer,
            "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                 &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                 &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);

            auto it = find(this->inodes.begin(),this->inodes.end(),inode);
            if (it != this->inodes.end()){
                struct Connection conn;
                inet_pton(AF_INET,local_addr,&conn.local_addr);
                inet_pton(AF_INET,rem_addr,&conn.remote_addr);
                conn.local_port = local_port;
                conn.remote_port = rem_port;
                nodetoprocess[inode] = conn;
            }
        }
    }
    while(!feof(procinfo));
    fclose(procinfo);
}


void Proc::setPid(unsigned int pid)
{
    this->pid = pid;
}

void Proc::printState()
{

    fetchInfoCpu();
    fetchInfoNetwork();
    scanTcp();

    while(1){
        system("clear");
        fetchInfoCpu();
        std::cout<<"--"<<this->pid<<std::endl;
        std::cout<<"parent ID:"<<this->ppid<<std::endl;
        std::cout<<"name: "<<this->name<<std::endl;
        std::cout<<"cpu used: "<<this->cpu_used<<std::endl;
        for ( auto x: this->cmdline) std::cout<<x <<endl;
        //    sleep(1);
        //    system("clear");
        //    }
        //    for (auto x:this->inodes){
        //        std::cout<<x<<std::endl;
        //    }
        for (auto x:this->nodetoprocess){
            //std::cout<<x.first<<std::endl;
            struct in_addr addr;
            addr.s_addr = (x.second).remote_addr;
            std::cout<<inet_ntoa(addr)<<std::endl;


        }
        //    getCmdline();
        //    for (auto x: this->cmdline) cout << x <<endl;
        sleep(1);
    }
    return;
}

void Proc::init()
{
    nodetoprocess.clear();
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time.cstime = 0;
    this->time.cutime = 0;
    this->time.stime = 0;
    this->time.utime = 0;
    this->time.last_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    std::string _spid = std::string(PATH_PROC) +"/" +std::to_string(this->pid) + std::string(PATH_STAT);
    char buffer[1<<14];

    FILE* procinfo = proc_fopen(_spid.c_str());
    if (procinfo == NULL)
    {
        std::cout <<"Process not found"<<std::endl;
        return;
    }


    int id, ppid, pgrp, session, tty_nr, tpgid, exit_signal, processor, exit_code;
    unsigned int flags, rt_priority, policy;
    long unsigned int minflt, cminflt, majflt, cmajflt, utime, stime, vsize, rsslim, startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap, guest_time, start_data, end_data, start_brk, arg_start, arg_end, env_start, env_end;
    long int cutime, cstime, priority, nice, num_threads, itrealvalue, rss, cguest_time;
    long long unsigned int starttime, delayacct_blkio_ticks;
    char name[PATH_MAX];
    char state;

    if (fgets(buffer, sizeof(buffer), procinfo))
        sscanf(buffer,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
               &id,name,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itrealvalue,&starttime,&vsize,&rss,&rsslim,&startcode,&endcode,&startstack,&kstkesp,&kstkeip,&signal,&blocked,&sigignore,&sigcatch,&wchan,&nswap,&cnswap,&exit_signal,&processor,&rt_priority,&policy,&delayacct_blkio_ticks,&guest_time,&cguest_time, &start_data, &end_data, &start_brk, &arg_start, &arg_end, &env_start, &env_end, &exit_code);
    fclose(procinfo);

    this->ppid = ppid;
    this->name = std::string(name);

    this->getCmdline();

}
