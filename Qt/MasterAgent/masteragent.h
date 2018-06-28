#ifndef MASTERAGENT_H
#define MASTERAGENT_H

#include <iostream>
#include <map>
#include <vector>

//slot to plug-in
typedef void (*slotFunc)();

typedef struct AgentInfo {
    std::string name;
    int memory;
    char state;
} AgentInfo;

static std::map<int,AgentInfo> listPid;
static std::vector<std::string> libList;

void newAgent(char name[]);
char checkAgent(pid_t pid);
void removeAgent(pid_t pid);
void printChildInfo();
int scanAgent();


#endif // MASTERAGENT_H
