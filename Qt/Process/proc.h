#ifndef PROC_H
#define PROC_H

#include <string>
#include <list>
#include <map>
#include <utility>
#include <arpa/inet.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>


#define PATH_FD_SUFF	"fd"
#define PATH_PROC_X_FD      PATH_PROC "/%s/" PATH_FD_SUFF
#define PATH_FD_SUFFl       strlen(PATH_FD_SUFF)
#define PATH_PROC "/proc"
#define PATH_STAT "/stat"
#define PATH_STATMEM "/statm"
#define PATH_FD "/fd"
#define _PATH_PROCNET_TCP "/proc/net/tcp"
#define _PATH_PROCNET_UDP "/proc/net/udp"
#define PATH_CMD "/cmdline"

#define PRG_LOCAL_ADDRESS "local_address"
#define PRG_INODE	 "inode"
#define PRG_SOCKET_PFX    "socket:["
#define PRG_SOCKET_PFXl (strlen(PRG_SOCKET_PFX))
#define PRG_SOCKET_PFX2   "[0000]:"
#define PRG_SOCKET_PFX2l  (strlen(PRG_SOCKET_PFX2))

using namespace std;

struct Connection{
    in_addr_t remote_addr;
    in_addr_t local_addr;
    in_port_t remote_port;
    in_port_t local_port;
};

typedef struct time_info{
    unsigned long long
    utime,		/* user-mode CPU time accumulated by process */
    stime,		/* kernel-mode CPU time accumulated by process */
    cutime,		/* cumulative utime of process and reaped children */
    cstime;	/* cumulative stime of process and reaped children */
    long long last_time; /*time in millsecond*/
} time_info;


class Proc
{
public:
    Proc();
    Proc(unsigned int pid);
    void setPid(unsigned int pid);
    void printState();
    unsigned int pid;

    bool operator == (unsigned int i) {return i == pid;}
private:

    unsigned int ppid;
    long unsigned int virtualMem;
    long unsigned int resMem;
    long unsigned int sharedMem;
    time_info time;
    float cpu_used;
    std::string name;
    std::list <std::string> cmdline;

    std::list <unsigned long> inodes;

    void init();

    void fetchInfoCpu();
    void fetchInfoNetwork();
    void fetchInfoMemory();

    void getCmdline();

    int extractInodeType1(const char lname[], unsigned long *inode);
    int extractInodeType2(const char lname[], unsigned long *inode);

    void scanTcp();
    void scanUdp();

    std::map<unsigned long,struct Connection> nodetoprocess;

};



#endif // PROC_H
