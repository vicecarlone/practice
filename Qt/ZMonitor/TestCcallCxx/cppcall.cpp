#include "cppcall.h"
#include <vector>

void * vector_create()
{
    return new std::vector<int>();
}

void vector_destroy(void * qist)
{
    delete (std::vector<int> *)qist;
}

void  vector_push_back(void * qist, int value)
{
    ((std::vector<int> *)qist)->push_back(value);
}

int vector_at(void * qist, int idx)
{
    std::vector<int> * q = (std::vector<int> *)qist;
    if(idx >= 0 && idx < q->size())
        return q->at(idx);
    return 0xFFFFFFFF;
}

void vector_pop(void * qist)
{
    ((std::vector<int> *)qist)->erase(((std::vector<int> *)qist)->begin());
}
