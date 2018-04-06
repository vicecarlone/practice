#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/select.h>
#include <vector>

int find_max_fd (int a, std::vector<int>){
    return a + 1;
}

void server(char *argv[]){
    int listenfd = 0;
    std::vector<int> connectfd;
    struct sockaddr_in serv_addr;

    int port = atoi(argv[2]);
    char sendBuff[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("binding failed");
        close(listenfd);
        return;
    }

    if (listen(listenfd, 10) < 0){
        printf("listen failed");
        close(listenfd);
        return;
    }

    fd_set rfds;

    FD_ZERO(&rfds);


    while(1)
    {
        struct timeval to = {0, 100000};
        int maxfd;
        int i;

        maxfd = find_max_fd(listenfd, connectfd);
        FD_SET(listenfd, &rfds);
        for(i = 0; i < connectfd.size(); i++)
        {
            FD_SET(connectfd[i], &rfds);
        }


        int idx = select(maxfd + 1, &rfds, NULL, NULL, &to);
        if(idx > 0)
        {
            if(FD_ISSET(listenfd, &rfds))
            {
                //OnAccept()
                int connfd = accept(listenfd, NULL, NULL);
                connectfd.push_back(connfd);
            }
            else
            {
                i = 0;
                while(i < connectfd.size())
                {
                    if(FD_ISSET(connectfd[i], &rfds))
                    {
                        int readbytes;
                        char buffer[2048];
                        readbytes = recv(connectfd[i], buffer, sizeof(buffer), 0);
                        if(readbytes == 0)//close
                        {

                            //OnCLose();
                            int sockfd = connectfd[i];
                            connectfd.erase(connectfd.begin() + i);
                        }
                        else
                        {
                            //OnReceive();
                            //process recv buffer;
                            send(connectfd[i], buffer, readbytes, 0);
                            i++;
                        }
                    }
                }
            }
        }
        //connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

//        ticks = time(NULL);
//        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
//        write(connfd, sendBuff, strlen(sendBuff));

//        close(connfd);
//        sleep(1);
     }
}

void client (char *argv[]){
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    int port = atoi(argv[2]);

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return;
    }

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return;
    }

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }

    if(n < 0)
    {
        printf("\n Read error \n");
    }

    return;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return -1;
    switch (argv[1][0]) {
    case 's':
        server(argv);
        break;
    case 'c':
        client(argv);
        break;
    default:
        break;
    }
    return 0;
}
