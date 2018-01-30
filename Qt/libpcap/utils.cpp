#include "utils.h"
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <algorithm>
#include <unistd.h>
#include <ctime>

typedef struct address{
    struct in_addr ip_src,ip_dst;
    u_short th_sport, th_dport;

    time_t lastime;
    bool operator==(const address &a) const{
        return (a.ip_dst.s_addr == ip_dst.s_addr && a.ip_src.s_addr == ip_src.s_addr &&  a.th_dport == th_dport && a.th_sport == th_sport);
    }

} address;

void * vector_create(){
    return new std::vector<address>;
}

void vector_destroy(void * queue){
    delete (std::vector<address>*) queue;
}

void element_push(void *queue, void * conn){
    address *instance  = (address *)conn;
    std::vector<address> * temp = (std::vector<address>*)queue;
    std::vector<address>::iterator it  =  std::find(temp->begin(),temp->end(),*instance);
    if (it == temp->end())
    ((std::vector<address> *) queue)->push_back(*instance);
}

int lookup(void *queue, struct in_addr ip_src,struct in_addr ip_dst, u_short th_sport,u_short th_dport){
    address instance;
    instance.ip_dst = ip_dst;
    instance.ip_src = ip_src;
    instance.th_dport= th_dport;
    instance.th_sport = th_sport;
    std::vector<address> * temp = (std::vector<address> *) queue;
    for (unsigned int i = 0; i < temp->size(); i++){
        if (temp->at(i) == instance)
            return i;
    }
    return -1;
}

void check_timeout(void * queue){
    std::vector<address> * temp= (std::vector<address>*)queue;

    Begin:
    for (unsigned int i = 0; i < temp->size(); i++){
        time_t now = time(0);
        address instance = temp->at(i);
        if (difftime(now,instance.lastime) > TIMEOUT){
            std::vector<address>::iterator it  =  std::find(temp->begin(),temp->end(),instance);
            if (it != temp->end())
                ((std::vector<address>*) queue)->erase(it);
            goto Begin;
        }
    }
}

void element_remove(void *queue,  struct in_addr ip_src,struct in_addr ip_dst, u_short th_sport,u_short th_dport){
    address *instance = new address;
    instance->ip_dst = ip_dst;
    instance->ip_src = ip_src;
    instance->th_dport= th_dport;
    instance->th_sport = th_sport;
    std::vector<address> * temp = (std::vector<address>*)queue;
    std::vector<address>::iterator it;
    it  =  std::find(temp->begin(),temp->end(),*instance);
    if (it != temp->end())
        ((std::vector<address>*) queue)->erase(it);
    delete instance;
}

void *element_create(in_addr ip_src, in_addr ip_dst, u_short th_sport, u_short th_dport)
{
    address *ele = new address;
    ele->ip_dst = ip_dst;
    ele->ip_src = ip_src;
    ele->th_dport = th_dport;
    ele->th_sport = th_sport;
    ele->lastime = time(0);
    return ele;
}

void print_queue(void *queue)
{
        std::vector<address> *temp = (std::vector<address>*) queue;
        if (temp->empty())
            return;
        for (unsigned int i = 0; i < temp->size(); i++){
            address instance = temp->at(i);
            printf("From: %14s:%d", inet_ntoa(instance.ip_src),ntohs(instance.th_sport));
            printf("\tTo: %s:%d\n", inet_ntoa(instance.ip_dst),ntohs(instance.th_dport));
        }

}
