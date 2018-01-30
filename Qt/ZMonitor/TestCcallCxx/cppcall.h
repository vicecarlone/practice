#ifndef CPPCALL_H
#define CPPCALL_H
#ifdef __cplusplus
extern "C" {
#endif

void * vector_create();
void   vector_destroy(void * qinst);
void  vector_push_back(void * qist, int value);
void  vector_pop(void * qist);
int vector_at(void * qist, int idx);

#ifdef __cplusplus
}
#endif

#endif // CPPCALL_H
