#include "masteragent.h"
#include "childagent.h"
#include "Poco/SharedLibrary.h"
#include "Poco/RegularExpression.h"
#include <unistd.h>
#include <limits>
#include <map>
#include <dirent.h>
#include <string>

#define PATH_PROC "/proc"
#define PROC_STAT "/stat"
#define PROC_STATM "/statm"

using Poco::SharedLibrary;
using Poco::RegularExpression;

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

std::string cwd(){
    char temp[PATH_MAX];
    return getcwd(temp,sizeof(temp)) ? std::string (temp) : std::string("")  ;
}

void newAgent(char name[]) {
    AgentInfo info;
    info.name = name;
    info.state = 'R';
    pid_t childPid = fork();
    if (childPid){
        listPid[childPid]=info;
    }
    else {
        ChildAgent child;
        child.run();
    }
}

void printChildInfo(){
    std::map<int, AgentInfo>::iterator it = listPid.begin();
    while(it != listPid.end()){
        AgentInfo info = it->second;
        std::cout <<"Agent name:" <<(info.name) <<std::endl;
        std::cout <<"\tAgent pid:"     << it->first <<std::endl;
        std::cout <<"\tAgent state:" << info.state <<std::endl;
        it++;
    }
}

void reviveAgent(pid_t pid){
    std::map<int, AgentInfo>::iterator it = listPid.find(pid);
    std::string path((it->second).name);
    path.append(Poco::SharedLibrary::suffix());
    SharedLibrary library(path);
    //
    //Do things
    //
    library.unload();
    return;
}

char checkAgent(pid_t pid){
    FILE* f;
    char path[15];
    char buffer[8192];
    sprintf(path,PATH_PROC "/%d"PROC_STAT,pid);
    f = proc_fopen(path);
    if (f == NULL)
    {
        fclose(f);
        return 'Z';
    }

    int id;
    char name[PATH_MAX];
    char state;

    /*read info from file proc/[pid]/stat*/
    if (fgets(buffer, sizeof(buffer), f))
        sscanf(buffer,"%d %s %c",
               &id,name,&state);
    fclose(f);
    return state;
}

int scanAgent()
{
    std::string pwd = cwd();
    DIR *dir;
    struct dirent *ent;
    RegularExpression rex("(.*).so");
    if ((dir = opendir (pwd.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            std::string temp(ent->d_name);
            bool match = rex.match(temp);
            if (match)
            {
                printf ("add %s to list\n", ent->d_name);
                libList.push_back(temp);

            }
        }
        closedir (dir);
        return 1;
    }
    else
    {
        perror ("");
        return 0;
    }
}
