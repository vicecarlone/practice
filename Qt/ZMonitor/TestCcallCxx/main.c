#include <stdio.h>
#include "cppcall.h"
int main(int argc, char *argv[])
{
    void * queue = vector_create();

    vector_push_back(queue, 0);
    vector_push_back(queue, 3);
    vector_push_back(queue, 2);

    int i;

    for(i = 0; i < 3; i++)
    {
        printf("%4d", vector_at(queue, i));
    }


    printf("\n");
    vector_pop(queue);
    for(i = 0; i < 2; i++)
    {
        printf("%4d", vector_at(queue, i));
    }
    vector_destroy(queue);
    return 0;
}
