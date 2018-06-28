#include <unistd.h>
#include <iostream>
#include <limits>

#include "childagent.h"
#include "masteragent.h"
#include "Poco/SharedLibrary.h"

int main(int argc, char *argv[])
{
    /*std::cout <<"Current Pid: " <<getpid() <<std::endl;
    newAgent("first");
    newAgent("second");
    system("sleep 1");
    printChildInfo();*/
    scanAgent();
    for (std::vector<std::string>::iterator it = libList.begin(); it != libList.end();){
        std::cout <<libList[0];
        it++;
    }

    printChildInfo();
}

