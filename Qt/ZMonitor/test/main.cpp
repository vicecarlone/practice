#include <iostream>
#include "ultis.h"
using namespace std;

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;

    Interfaces itf;

    itf.filt_ethernet();

    cout << itf.ethernet[0];

    Capturer cap(itf.ethernet[0]);
    cap.capture();
    return 0;
}
