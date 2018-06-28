#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>
#include <thrift/transport/TFileTransport.h>
#include <thrift/stdcxx.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<sys/uio.h>
#include<unistd.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<string.h>
#include<stdio.h>

#include <gen-cpp/VersionChecker.h>
#include <gen-cpp/VersionChecker_types.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using namespace VersionChecker;

using namespace std;


class VersionHandler : public VersionCheckerIf {
public:
    VersionHandler(){}
    ~VersionHandler(){}
    void fetchVersion(Version& _return,const Version& ver){




        _return.version = 1;
        _return._major = 0;
        _return._minor = 0;
        _return.additional = 1;
        return;
    }
    void getFile(){

        int PORT = 7777, BACKLOG = 5;
        int fd1,fd2;
        int bnd,lstn;
        char buf[100]={' '};
        struct sockaddr_in server,client;

        fd1=socket(AF_INET,SOCK_STREAM,0);
        if(fd1<0){
          cout<<"Error creating socket\n";
          return;
        }
        cout<<"Socket created\n";

        server.sin_family=AF_INET;
        server.sin_port=htons(PORT);
        server.sin_addr.s_addr=INADDR_ANY;


        bnd=bind(fd1,(struct sockaddr *)&server,sizeof(server));
        if(bnd<0){
          cout<<"Error binding\n";
          return;
        }

        lstn=listen(fd1,BACKLOG);
        if(lstn<0){
          cout<<"Error listening\n";
          return;
        }

        cout<<"Server is listening\n";
        sleep(1);
        socklen_t len=sizeof(client);

        fd2=accept(fd1,(struct sockaddr*)&client,&len);
        if(fd2<0){
          cout<<"Error accepting\n";
          return;
        }


        int from;
        from=open("test.o",O_RDONLY);
        if(from<0){
          cout<<"Error opening file\n";
          return;
        }
        int n=1;
        int s;
        while((n=read(from,buf,sizeof(buf)))!=0){
          //s=send(fd2,buf,sizeof(buf),0);
          s=write(fd2,buf,n);
          if(s<0){cout<<"error sending\n";return;}
        }

        close(fd2);

        close(fd1);
        shutdown(fd1,0);
        shutdown(fd2,0);
        std::cout <<"Closing"<<endl;

    }
};

class VersionCloneFactory : virtual public VersionCheckerIfFactory {
    public:
        VersionCloneFactory(const VersionCloneFactory&){}
        VersionCloneFactory(){}
        virtual ~VersionCloneFactory() {}
        virtual VersionCheckerIf* getHandler(const TConnectionInfo &connInfo){
            return new VersionHandler();
        }
        virtual void releaseHandler(VersionCheckerIf* handler){
            delete handler;
        }
};


int main()
{
    int port = 9090;

    shared_ptr<VersionHandler> handler(new VersionHandler);
    shared_ptr<TProcessor> processor(new VersionCheckerProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());


    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
    return 0;
}


