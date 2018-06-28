#include <iostream>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/Dynamic/Var.h>

using namespace std;
using namespace Poco::JSON;
using namespace Poco::Dynamic;

int main(int argc, char *argv[])
{
    Poco::JSON::Object::Ptr inner = new Poco::JSON::Object;
    inner->set("some_number", 5);
    inner->set("some_string", "xyz");

    Var number = inner->get("some_number");
    Var string = inner->get("some_string");
    cout<< number.convert<std::string>()<<endl;
    cout<< string.convert<std::string>()<<endl;

    return 0;
}
