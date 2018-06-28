#include <iostream>
#include "gen-cpp/VersionChecker.h"
#include "gen-cpp/VersionChecker_types.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/stdcxx.h>

#include<fcntl.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace VersionChecker;

int transFile(char* argv){
    int fd, PORT = 7777;
    struct sockaddr_in server;
    struct hostent *hp;
    int cnct;
    char buf[100]={' '};

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=INADDR_ANY;

    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){
      cout<<"Error creating socket\n";
      return 0;
    }

    cout<<"Socket created\n";

    hp=gethostbyname(argv);
    bcopy((char *)hp->h_addr,(char *)&server.sin_addr.s_addr,hp->h_length);

    cnct=connect(fd,(struct sockaddr*)&server,sizeof(server));

    if(cnct<0){
      cout<<"Error connecting\n";
      return 0;
    }

    cout<<"Connection has been made\n";
    int rec;

    int to;
    to=creat("output.txt",0777);
    if(to<0){
      cout<<"Error creating destination file\n";
      return 0;
    }
    int w;
    while(rec=recv(fd,buf,sizeof(buf),0)){
      if(rec<0){
        cout<<"Error receiving\n";
        return 0;
      }
      w=write(to,buf,rec);
    }
    close(fd);
    shutdown(fd,0);
    cout<<"Socket closed\n";

    return 0;
}



int main(int argc, char *argv[])
{
    stdcxx::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    stdcxx::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    stdcxx::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    VersionCheckerClient client(protocol);

    try {
        Version ver;
        Version _return;
        transport->open();
        client.fetchVersion(_return,ver);
        cout << _return.version <<"." <<_return._major << "." <<_return._minor <<"." <<_return.additional<<endl;

        client.getFile();
        transFile("localhost");

        transport->close();
    }
    catch (TException &tx){
        cout << tx.what() <<endl;
    }

    return 0;
}


