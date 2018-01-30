#include "zprocess.h"
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





void readMem(char *pid)
    {
    FILE *procinfo;
    char path[13];
    char buffer[8192];
    sprintf(path,PATH_PROC "/%s"PROC_STATM,pid);
    procinfo = proc_fopen(path);
    if (procinfo == NULL)
        return;
    long unsigned int vSize, resSize, sharedSize;

    if (fgets(buffer,sizeof(buffer), procinfo))
        sscanf(buffer,"%lu %lu %lu",&vSize,&resSize,&sharedSize);

    fclose(procinfo);
    printf("\t %10lu \t %10lu \t %10lu \n",vSize,resSize,sharedSize);
}

int readProc(char* pid, time_info* ltask)
{


    FILE *procinfo;
    char path[13];
    char buffer[8192];
    sprintf(path,PATH_PROC "/%s"PROC_STAT,pid);
    procinfo = proc_fopen(path);
    struct timeval tv;

    if (procinfo == NULL)
        return -1;

    int id, ppid, pgrp, session, tty_nr, tpgid, exit_signal, processor, exit_code;
    unsigned int flags, rt_priority, policy;
    long unsigned int minflt, cminflt, majflt, cmajflt, utime, stime, vsize, rsslim, startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap, guest_time, start_data, end_data, start_brk, arg_start, arg_end, env_start, env_end;
    long int cutime, cstime, priority, nice, num_threads, itrealvalue, rss, cguest_time;
    long long unsigned int starttime, delayacct_blkio_ticks;
    char name[PATH_MAX];
    char state;


    /*read info from file proc/[pid]/stat*/
    if (fgets(buffer, sizeof(buffer), procinfo))
        sscanf(buffer,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
               &id,name,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itrealvalue,&starttime,&vsize,&rss,&rsslim,&startcode,&endcode,&startstack,&kstkesp,&kstkeip,&signal,&blocked,&sigignore,&sigcatch,&wchan,&nswap,&cnswap,&exit_signal,&processor,&rt_priority,&policy,&delayacct_blkio_ticks,&guest_time,&cguest_time, &start_data, &end_data, &start_brk, &arg_start, &arg_end, &env_start, &env_end, &exit_code);
    fclose(procinfo);



    time_info *temp = ltask;
    bool found = false;
    float cpu_used = 0.0f;


    /*calculate cpu_used and update list of tasks*/
    while (temp != NULL){
        if (temp->pid == id){
            found = true;
            gettimeofday(&tv, NULL);
            long long ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
            cpu_used = (float)(utime + stime + cstime + cutime - temp->cstime - temp->cutime - temp->stime - temp->utime) / (float)sysconf(_SC_CLK_TCK) /(ms - temp->last_time) * 1000.0f;
            temp->cstime = cstime;
            temp->cutime = cutime;
            temp->stime = stime;
            temp->utime = utime;
            temp->last_time = ms;
            break;
        }
        else
            temp = temp->next;
    }

    /*add a node to list of tasks if a new pid been found*/
    if (!found){
        temp = ltask;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = (time_info*) malloc (sizeof(time_info));
        temp->next->pid = id;
        temp->next->next = NULL;
        temp->next->cstime = cstime;
        temp->next->cutime = cutime;
        temp->next->stime = stime;
        temp->next->utime = utime;
        gettimeofday(&tv, NULL);
        long long ms = tv.tv_sec * 1000ll + tv.tv_usec / 1000;
        temp->next->last_time = ms;
    }

    /*print out*/
    printf("%d \t %20.18s \t %c \t %f ",id,name,state,cpu_used*100.0f/sysconf(_SC_NPROCESSORS_ONLN));
    readMem(pid);
    return 1;
}


void getProc(){
    const char *cs;

    time_info *lst_task = (time_info*)malloc(sizeof(time_info));
    lst_task->next = NULL;
    lst_task->pid = -1;

    for(;;){
        DIR *dirproc = NULL;
        struct dirent *proc;
        if (!(dirproc=opendir(PATH_PROC))) return;
        while(errno = 0, proc = readdir(dirproc)){
            for (cs = proc->d_name; *cs; cs++)
                if (!isdigit(*cs))
                break;
            if (*cs)
                continue;
            readProc(proc->d_name, lst_task);
        }
        usleep(interval);
        system("clear");
    }
}
