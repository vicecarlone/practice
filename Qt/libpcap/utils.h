#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>

#define TIMEOUT 2.0000
#define REFRESH_OUTPUT 1000000

#ifdef __cplusplus
extern "C" {
#endif

void element_push(void * queue, void * conn);

int lookup(void * queue, struct in_addr ip_src,struct in_addr ip_dst, u_short th_sport,u_short th_dport);

void element_remove(void * queue, struct in_addr ip_src,struct in_addr ip_dst, u_short th_sport,u_short th_dport);

void * vector_create();

void * element_create(struct in_addr ip_src,struct in_addr ip_dst, u_short th_sport,u_short th_dport);

void vector_destroy(void * queue);

void print_queue(void * queue);

void check_timeout(void * queue);
#ifdef __cplusplus
}
#endif

#endif // UTILS_H
