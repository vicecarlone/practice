#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>

#define PATH_PROC "/proc/"

#define PATH_PID(x) PATH_PROC x 
/*
** getsocktop commands
*/
#define NETATOP_BASE_CTL 15661

// just probe if the netatop module is active
#define NETATOP_PROBE (NETATOP_BASE_CTL)

// force garbage collection to make finished processes available
#define NETATOP_FORCE_GC (NETATOP_BASE_CTL + 1)

// wait until all finished processes are read (blocks until done)
#define NETATOP_EMPTY_EXIT (NETATOP_BASE_CTL + 2)

// get info for finished process (blocks until available)
#define NETATOP_GETCNT_EXIT (NETATOP_BASE_CTL + 3)

// get counters for thread group (i.e. process):  input is 'id' (pid)
#define NETATOP_GETCNT_TGID (NETATOP_BASE_CTL + 4)

// get counters for thread:  input is 'id' (tid)
#define NETATOP_GETCNT_PID (NETATOP_BASE_CTL + 5)

#define COMLEN 16
#define PNAMLEN 15
#define CMDLEN 4096

typedef unsigned long long count_t;

static int sock = -1;

//Struct use for netatop service//
struct netcount
{
	unsigned long long tcpsndpacks;
	unsigned long long tcpsndbytes;
	unsigned long long tcprcvpacks;
	unsigned long long tcprcvbytes;

	unsigned long long udpsndpacks;
	unsigned long long udpsndbytes;
	unsigned long long udprcvpacks;
	unsigned long long udprcvbytes;

	/* space for future extensions */
};

//Structures use for gathering system information//

/* NETWORK STATISTICS						*/
struct net
{
	count_t tcpsnd;		/* number of TCP-packets sent	*/
	count_t tcpssz;		/* cumulative size packets sent	*/
	count_t tcprcv;		/* number of TCP-packets recved	*/
	count_t tcprsz;		/* cumulative size packets rcvd	*/
	count_t udpsnd;		/* number of UDP-packets sent	*/
	count_t udpssz;		/* cumulative size packets sent	*/
	count_t udprcv;		/* number of UDP-packets recved	*/
	count_t udprsz;		/* cumulative size packets sent	*/
	count_t avail1;		/* */
	count_t avail2;		/* */
	count_t cfuture[4]; /* reserved for future use	*/
};
/* GENERAL TASK INFO 					*/
struct gen
{
	int tgid;				  /* threadgroup identification 	*/
	int pid;				  /* process identification 	*/
	int ppid;				  /* parent process identification*/
	int ruid;				  /* real  user  identification 	*/
	int euid;				  /* eff.  user  identification 	*/
	int suid;				  /* saved user  identification 	*/
	int fsuid;				  /* fs    user  identification 	*/
	int rgid;				  /* real  group identification 	*/
	int egid;				  /* eff.  group identification 	*/
	int sgid;				  /* saved group identification 	*/
	int fsgid;				  /* fs    group identification 	*/
	int nthr;				  /* number of threads in tgroup 	*/
	char name[PNAMLEN + 1];   /* process name string       	*/
	char isproc;			  /* boolean: process level?      */
	char state;				  /* process state ('E' = exited)	*/
	int excode;				  /* process exit status		*/
	time_t btime;			  /* process start time (epoch)	*/
	time_t elaps;			  /* process elaps time (hertz)	*/
	char cmdline[CMDLEN + 1]; /* command-line string       	*/
	int nthrslpi;			  /* # threads in state 'S'       */
	int nthrslpu;			  /* # threads in state 'D'       */
	int nthrrun;			  /* # threads in state 'R'       */

	int ctid; /* OpenVZ container ID		*/
	int vpid; /* OpenVZ virtual PID		*/

	int wasinactive; /* boolean: task inactive	*/

	char container[16]; /* Docker container id (12 pos)	*/
};
//
struct sysstat
{
	struct gen gen;
	struct net net;
};

struct netpertask
{
	pid_t id; // tgid or tid (depending on command)
	unsigned long btime;
	char command[COMLEN];

	struct netcount tc;
};

void opensock(void)
{
	/*opne socket*/
	sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
}

int netatop_probe(void)
{
	socklen_t sl = 0;

	/*check weather socket open yet*/
	if (sock == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	if (getsockopt(sock, SOL_IP, NETATOP_PROBE, NULL, &sl))
	{
		printf("service netatop is not started\n");
		return -1;
	}
	return 0;
}

void netatop_gettask(pid_t pid, char type, struct net *sstat)
{
	struct netpertask npt;
	socklen_t socklen = sizeof npt;
	int cmd = (type == 'g' ? NETATOP_GETCNT_TGID : NETATOP_GETCNT_PID);

	npt.id = pid;
	printf("%d", pid);
	setuid(0);
	if (getsockopt(sock, SOL_IP, cmd, &npt, &socklen) != 0)
	{
		memset(&sstat, 0, sizeof sstat);
		//printf("can this reach here?\n");
		printf("%s\n", strerror(errno));

		return;
	}
	printf("core dumped?\n");

	printf("%lld\n", npt.tc.tcpsndpacks);
	printf("%lld\n", npt.tc.tcprcvpacks);
	printf("%lld\n", npt.tc.tcpsndbytes);
	printf("%lld\n", npt.tc.tcprcvbytes);

	printf("%lld\n", npt.tc.udpsndpacks);
	printf("%lld\n", npt.tc.udprcvpacks);
	printf("%lld\n", npt.tc.udpsndbytes);
	printf("%lld\n", npt.tc.udprcvbytes);
}

int getNumPid(void)
{
	const char *cs;
	int index = 0;
	DIR *dirproc = NULL;
	struct dirent *proc;
	if (!(dirproc = opendir(PATH_PROC)))
		return -1;
	while (errno = 0, proc = readdir(dirproc))
	{
		for (cs = proc->d_name; *cs; cs++)
			if (!isdigit(*cs))
				break;
		if (*cs)
			continue;
		//listProc[index] = atoi(proc->d_name);
		index++;
	}
	return index;
}

void processFetch(char *pid,struct sysstat* task)
{
	
}

void generalInfo(struct sysstat *task, int maxtask)
{
	static int			firstcall = 1;
	static unsigned long long	bootepoch;

	register struct sysstat	*curtask;

	FILE		*fp;
	DIR		*dirp;
	struct dirent	*entp;
	char		origdir[1024], dockstat=0;
	int		tval=0, taskLen = sizeof(struct sysstat);
	

	dirp = opendir("/proc/");

	while( (entp = readdir(dirp)) && tval < maxtask )
	{
		/*
		** skip non-numerical names
		*/
		if (!isdigit(entp->d_name[0]))
			continue;

		/*
		** change to the process' subdirectory
		*/
		

 
		curtask = task + tval * taskLen;
		processFetch(entp->d_name,curtask);

		curtask->gen.pid = atoi(entp->d_name);
		netatop_gettask(curtask->gen.pid,'e',&curtask->net);


		tval++;
	}

	closedir(dirp);
}


int main(int argc, char **argv)
{
	int numProc = getNumPid();
	static struct sysstat *curstat;
	curstat = calloc(numProc,sizeof(struct sysstat));

	opensock();
	
	generalInfo(curstat,500);

	//printf("%lld\n",stat->tcprcv);

	close(sock);
	return 0;
}
