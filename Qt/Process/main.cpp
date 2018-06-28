#include <iostream>
#include "proc.h"
#include <list>
#include <limits.h>
#include <dirent.h>
#include <string.h>

using namespace std;



void procscan(list<Proc> &ls);

int main(int argc, char *argv[])
{
     /*
    cout <<"Enter the pid"<<endl;
    int pid;
    cin >> pid;
    Proc *proc = new Proc(pid);
    proc->printState();*/

    //procscan();
    list<Proc> lst;
    procscan(lst);

    for (auto x : lst) cout<<x.pid<<endl;
    return 0;
}
char *safe_strncpy(char *dst, const char *src, size_t size)
{
    dst[size-1] = '\0';
    return strncpy(dst,src,size-1);
}
void procscan(list<Proc> &ls){
    string _path;

    char line[LINE_MAX];
    int procfdlen;
    char cmdlbuf[512];
    const char *cs;
    DIR *dirproc = NULL;
    struct dirent *direproc;

    cmdlbuf[sizeof(cmdlbuf) - 1] = '\0';

    if(!(dirproc = opendir("/proc"))) goto fail;

    while( direproc = readdir(dirproc) ){\
        for (cs = direproc->d_name; *cs; cs++)
            if (!isdigit(*cs))
            break;
        if (*cs)
            continue;
        procfdlen = snprintf(line,sizeof(line),PATH_PROC_X_FD,direproc->d_name);
        if (procfdlen <= 0 || procfdlen >= sizeof(line) - 5)
            continue;
        \
        unsigned int pid = atoi(direproc->d_name);
        bool found = false;
        for ( auto x : ls){
            if (x == pid) found = true;
        }
        if (!found){
            Proc pr = Proc(pid);
            ls.push_back(pr);
        }

    }

    closedir(dirproc);
    return;

    fail:
    exit(0);

}
