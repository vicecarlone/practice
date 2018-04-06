#include "/home/cpu10761-local/Downloads/poco/Foundation/include/Poco/DynamicAny.h"

#include "/home/cpu10761-local/Downloads/poco/Foundation/include/Poco/Exception.h"
using Poco::DynamicAny;
int main(int argc, char** argv)
{
    DynamicAny any(42);
    int i = any;
    std::string s(any.convert<std::string>());
    any.convert(s); // or without the need to cast
    const int& ri(any.extract<int>());
    short s = any;
    try
    {
        short s = any.extract<short>(); // throws BadCastException
    }
    catch (Poco::BadCastException&)
    {
    }
    return 0;
}
