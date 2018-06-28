#include "childagent.h"
#include <unistd.h>
#include <iostream>


void ChildAgent::run()
{
    std::cout << "Current Pid: " <<getpid() <<std::endl;
    exit(1);
}
