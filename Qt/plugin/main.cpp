#include "Poco/SharedLibrary.h"
#include <iostream>
using Poco::SharedLibrary;
using namespace std;

/*plugin entry point*/
typedef void (*HelloFunc)(int *);

int main(int argc, char** argv)
{
    int x = 0;

    std::string path("/home/tuantu/Desktop/practice/Qt/build-plugin-Desktop_Qt_5_7_0_GCC_64bit-Debug/TestLibrary");
    path.append(Poco::SharedLibrary::suffix());
    std::cout << 1 <<endl;
    SharedLibrary library(path);
    std::cout <<2 <<endl;
    HelloFunc func = (HelloFunc) library.getSymbol("hello");
    std::cout <<3 <<endl;
    func(&x);
    library.unload();

    return 0;
}
