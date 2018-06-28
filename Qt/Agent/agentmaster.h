#ifndef AGENTMASTER_H
#define AGENTMASTER_H

#include <iostream>
#include <stdio.h>
#include "Poco/SharedLibrary.h"
#include <dirent.h>
#include <vector>
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <fstream>
#include <sys/stat.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/stdcxx.h>
#include <thrift/transport/TFileTransport.h>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/FileStream.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Util/ServerApplication.h"


#include "/home/tuantu/Desktop/apps/server/gen-cpp/Zserver.h"

#define AGENTCHILD_FORDER  "/home/tuantu/Desktop/apps/AgentChild/bin/"
#define AGENTMASTER_FORDER "/home/tuantu/Desktop/apps/server/gen-cpp/"
#define FILE_LOG "/home/tuantu/Desktop/apps/server/gen-cpp/"
#define FILE_CONFIG "/home/tuantu/Desktop/apps/AgentChild/config/"

#define CHILD_STOP 0

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace zserver;

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::JSON;

using Poco::SharedLibrary;
typedef int (*HelloFunc)(int argc, char **argv);

struct process
{
    int p_pid;
    string p_name;
    int num1;
    int num2;
    int num3;
    int type;
    int time;
    string arg;
    int status;
    time_t time_death;
};

class AgentMaster : public Poco::Util::ServerApplication
{
public:
    AgentMaster();
    virtual ~AgentMaster();
    int main(const std::vector<string> &args);

private:
    std::vector<process> listLibs;
    std::vector<process> listProcess;

    void restart(ZserverClient client);
    void checkNew();
    int  startAll();
    void checkState();
    void update(ZserverClient client);
    process checkAgentMaster();
    vector<process> check();
    vector<process> scan();
    vector<string> explode(const string& s, const char& c);
    void onRun();
    vector<process> readLog();
    void writeLog();
    void printState();
    void create(process p);
    void setPath(std::string);

};
#define AgentMasterInst (dynamic_cast<AgentMaster&> (AgentMaster::instance()))
#endif // AGENTMASTER_H
