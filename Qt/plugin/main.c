
#include <stdio.h>
#include <dlfcn.h>

#include "hello.h"

int main(int argc, char *argv[])
{
    void *lib_handle;
    int x = 0;

    lib_handle = dlopen("/home/tuantu/hello.so",RTLD_LAZY);
    if (!lib_handle){
	printf("0");
        return;
	}


    hello = (void (*)(int*))dlsym(lib_handle,"hello");
    if (dlerror() != NULL){
	printf("1");
        return;
    }
    (*hello)(&x);

    dlclose(lib_handle);

}
