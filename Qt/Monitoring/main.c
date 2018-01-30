#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>

#include "ultis.h"

int main(int argc, char *argv[])
{
    parameterParser(argc,argv);
    getProc();
    //readSocket(1);
    return 0;
}
