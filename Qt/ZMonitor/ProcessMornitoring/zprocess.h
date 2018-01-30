#ifndef ZPROCESS_H
#define ZPROCESS_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <stddef.h>
#include <limits.h>
#include <dirent.h>
#include <stdbool.h>
#include <time.h>


#define PATH_PROC "/proc"
#define PROC_STAT "/stat"
#define PROC_STATM "/statm"

#define SIGNAL_STRING

unsigned long interval = 1000000; /*time interval in microsecond*/

/*struct time info to calculate cpu_used*/
typedef struct time_info{
    int pid;
    int isFirst;
    unsigned long long
    utime,		/* user-mode CPU time accumulated by process */
    stime,		/* kernel-mode CPU time accumulated by process */
// and so on...
    cutime,		/* cumulative utime of process and reaped children */
    cstime;	/* cumulative stime of process and reaped children */
    long long last_time; /*time in millsecond*/
    struct time_info *next;
} time_info;



/*struct to minin file /proc/[pid]/stat*/
typedef struct proc_t {
// 1st 16 bytes
    int
        pid,		/* process id */
        ppid;		/* pid of parent process */
    unsigned
        pcpu;           /* %CPU usage (is not filled in by readproc!!!) */
    char
        state,		/* single-char code for process state (S=sleeping) */
        pad_1,		/* padding */
        pad_2,		/* padding */
        pad_3;		/* padding */
// 2nd 16 bytes
    unsigned long long
    utime,		/* user-mode CPU time accumulated by process */
    stime,		/* kernel-mode CPU time accumulated by process */
// and so on...
    cutime,		/* cumulative utime of process and reaped children */
    cstime,		/* cumulative stime of process and reaped children */
    start_time;	/* start time of process -- seconds since 1-1-70 */
#ifdef SIGNAL_STRING
    char
    /* Linux 2.1.7x and up have 64 signals. Allow 64, plus '\0' and padding. */
    signal[18],	/* mask of pending signals */
    blocked[18],	/* mask of blocked signals */
    sigignore[18],	/* mask of ignored signals */
    sigcatch[18];	/* mask of caught  signals */
#else
    long long
    /* Linux 2.1.7x and up have 64 signals. */
    signal,		/* mask of pending signals */
    blocked,	/* mask of blocked signals */
    sigignore,	/* mask of ignored signals */
    sigcatch;	/* mask of caught  signals */
#endif
    long
    priority,	/* kernel scheduling priority */
    timeout,	/* ? */
    nice,		/* standard unix nice level of process */
    rss,		/* resident set size from /proc/#/stat (pages) */
    it_real_value,	/* ? */
    /* the next 7 members come from /proc/#/statm */
    size,		/* total # of pages of memory */
    resident,	/* number of resident set (non-swapped) pages (4k) */
    share,		/* number of pages of shared (mmap'd) memory */
    trs,		/* text resident set size */
    lrs,		/* shared-lib resident set size */
    drs,		/* data resident set size */
    dt;		/* dirty pages */
    unsigned long
    vm_size,        /* same as vsize in kb */
    vm_lock,        /* locked pages in kb */
    vm_rss,         /* same as rss in kb */
    vm_data,        /* data size */
    vm_stack,       /* stack size */
    vm_exe,         /* executable size */
    vm_lib,         /* library size (all pages, not just used ones) */
    rtprio,		/* real-time priority */
    sched,		/* scheduling class */
    vsize,		/* number of pages of virtual memory ... */
    rss_rlim,	/* resident set size limit? */
    flags,		/* kernel flags for the process */
    min_flt,	/* number of minor page faults since process start */
    maj_flt,	/* number of major page faults since process start */
    cmin_flt,	/* cumulative min_flt of process and child processes */
    cmaj_flt,	/* cumulative maj_flt of process and child processes */
    nswap,		/* ? */
    cnswap,		/* cumulative nswap ? */
    start_code,	/* address of beginning of code segment */
    end_code,	/* address of end of code segment */
    start_stack,	/* address of the bottom of stack for the process */
    kstk_esp,	/* kernel stack pointer */
    kstk_eip,	/* kernel instruction pointer */
    wchan;		/* address of kernel wait channel proc is sleeping in */
    char
    **environ,	/* environment string vector (/proc/#/environ) */
    **cmdline;	/* command line string vector (/proc/#/cmdline) */
    char
    /* Be compatible: Digital allows 16 and NT allows 14 ??? */
        ruser[16],	/* real user name */
        euser[16],	/* effective user name */
        suser[16],	/* saved user name */
        fuser[16],	/* filesystem user name */
        rgroup[16],	/* real group name */
        egroup[16],	/* effective group name */
        sgroup[16],	/* saved group name */
        fgroup[16],	/* filesystem group name */
        cmd[16];	/* basename of executable file in call to exec(2) */
    int
        ruid, rgid,     /* real      */
        euid, egid,     /* effective */
        suid, sgid,     /* saved     */
        fuid, fgid,     /* fs (used for file access only) */
    pgrp,		/* process group id */
    session,	/* session id */
    tty,		/* full device number of controlling terminal */
    tpgid,		/* terminal process group id */
    exit_signal,	/* might not be SIGCHLD */
    processor;      /* current (or most recent?) CPU */
#ifdef FLASK_LINUX
    security_id_t sid;
#endif
} proc_t;


void getProc();

#endif // ZPROCESS_H
