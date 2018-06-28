#include "agentmaster.h"

using namespace std;

AgentMaster::AgentMaster()
{
    this->listLibs = {};
    this->listProcess = {};
}

AgentMaster::~AgentMaster()
{

}

void handler (int sgn){
    pid_t pid;
    pid = wait(NULL);
}

void AgentMaster::onRun()
{
    stdcxx::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    stdcxx::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    stdcxx::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    ZserverClient client(protocol);

    signal(SIGCHLD,handler);
    listProcess = readLog();

    if(listProcess.empty())
    {
        printf("empty \n");
        this->listLibs = scan();
        int child = startAll();
        if (child == 0)
        {
            goto stop_child;
        }
    }
    else {
        this->listLibs= scan();
        for (int i = 0; i < int(listLibs.size()); ++i) {
            printf("%s %d. %d. %d \n", listLibs[i].p_name.c_str(), listLibs[i].num1, listLibs[i].num2, listLibs[i].num3);
        }

        //checkNew();
    }
    checkState();

    printf("====================================\n");
    cout<<"number process   "<<listProcess.size()<<endl;
    printState();
    /*
    try {
        transport->open();

        while (true)
        {
            printf("start \n");
            sleep(5);
            checkState();
            checkNew();
            client.ping();
            update(client);
            //writeLog();
            //restart(client);
        }
        transport->close();
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }
    */
stop_child:
    printf("stop child \n" );
}

FILE *proc_fopen(const char *name)
{
    static char *buffer;
    static size_t pagesz;
    FILE *fd = fopen(name, "r");

    if (fd == NULL)
        return NULL;

    if (!buffer) {
        pagesz = getpagesize();
        buffer = (char*) (malloc(pagesz));
    }

    setvbuf(fd, buffer, _IOFBF, pagesz);
    return fd;
}

vector<process> AgentMaster::readLog()
{
    string logFile ="log.txt";
    string fInfo(FILE_LOG + logFile);
    FILE* log = proc_fopen(fInfo.c_str());

    if (log == NULL){
        cout<<"Cant open file log"<<endl;
        return {} ;
    }

    Parser parser;
    char buffer[8196];
    vector<process> listProcess;

    if (fgets(buffer, sizeof(buffer), log)){
        std::string content = std::string(buffer);
        cout<<content<<endl;
        Poco::Dynamic::Var result = parser.parse(content);
        Array::Ptr arr = result.extract<Array::Ptr>();
        Object::Ptr object = arr->getObject(0);

        for (int i = 0; i < int(arr->size()); ++i) {
            Object::Ptr object = arr->getObject(i);
            int p_pid = object->getValue<int>("p_pid");
            string p_name = object->getValue<string>("p_name");
            int num1 = object->getValue<int>("num1");
            int num2 = object->getValue<int>("num2");
            int num3 = object->getValue<int>("num3");
            process p;
            p.p_pid = p_pid;
            p.p_name = p_name;
            p.num1 = num1;
            p.num2 = num2;
            p.num3 = num3;
            listProcess.push_back(p);
        }
    }
    fclose(log);
    return listProcess;
}

void AgentMaster::writeLog()
{
    ofstream myfile;
    string logFile = "log.txt";
    myfile.open (FILE_LOG + logFile);

    if (!myfile.is_open()){
        cout<<"Cant open file log"<<endl;
        return;
    }

    Array::Ptr arr = new Array();
    for (int i = 0; i < int(listProcess.size()); ++i) {
        Object::Ptr object = new JSON::Object;
        object->set("p_pid", listProcess[i].p_pid);
        object->set("p_name", listProcess[i].p_name);
        object->set("num1", listProcess[i].num1);
        object->set("num2", listProcess[i].num2);
        object->set("num3", listProcess[i].num3);
        object->set("type", listProcess[i].type);
        object->set("time", listProcess[i].time);
        object->set("arg", listProcess[i].arg);
        object->set("status", listProcess[i].status);

        arr->set(i, object);
    }

    stringstream out;
    Stringifier *stringifier = new Stringifier();
    stringifier->stringify(arr, out);

    string json = out.str();
    myfile<<json<<endl;
}

void AgentMaster::printState()
{
    printf("Pid \t Process name \t Version \t Type \t Time \t Argument \t \t \t Action \t Status  \n");
    for (int i = 0; i < int(listProcess.size()); ++i) {
        printf("%d \t %s \t %d.%d.%d \t \t %d \t %d \t %s \t \t %d \t\t %d \n", listProcess[i].p_pid, listProcess[i].p_name.c_str(), listProcess[i].num1, listProcess[i].num2, listProcess[i].num3
               , listProcess[i].type, listProcess[i].time, listProcess[i].arg.c_str(), listProcess[i].status, kill(listProcess[i].p_pid, 0));
    }
    printf("-----------------------------------------------------------------\n");
}

void AgentMaster::create(process p)
{
    string lib_name = p.p_name + "-" + to_string(p.num1) + "." + to_string(p.num2)+ "." + to_string(p.num3);

    vector<string> tmp = explode(p.arg, ' ');
    char **argv = new char*[tmp.size()];
    int i=0;
    for (auto n:tmp)
    {
        argv[i] = new char[n.length() + 1];
        std::strcpy(argv[i], n.c_str());
        i++;
    }
    int argc = tmp.size();

    string path(AGENTCHILD_FORDER + lib_name);
    path.append(SharedLibrary::suffix());
    SharedLibrary library(path);
    HelloFunc func = (HelloFunc) library.getSymbol("main");
    func(argc, argv);
    library.unload();
}

void AgentMaster::restart(ZserverClient client)
{
    string file_new ="";
    process p = checkAgentMaster();
    printf("master %s %d %d %d \n", p.p_name.c_str(), p.num1, p.num2, p.num3);
    client.check(file_new, p.p_name, p.num1, p.num2, p.num3);
    if (file_new != "")
    {
        printf("Agent master new %s \n", file_new.c_str());
restart:
        pid_t pid =fork();
        if (pid == 0)
        {
            bool get_new = client.getNewVersion(file_new);
            if (get_new)
            {
                string file_old = p.p_name + "-" + to_string(p.num1) + "." + to_string(p.num2) + "." + to_string(p.num3);
                string file_rv = AGENTMASTER_FORDER + file_old;
                remove(file_rv.c_str());
                exit(0);
            }
        }
        else if (pid > 0)
        {
            sleep(1);
            string reciver = AGENTMASTER_FORDER + file_new;
            StreamSocket ss;
            ss.connect(SocketAddress("localhost", 9000));
            SocketStream istr(ss);
            string file(reciver);
            Poco::FileOutputStream ostr(file, std::ios::binary);
            StreamCopier::copyStream(istr, ostr);
            printf("Update MasterAgent complete %s \n", reciver.c_str());
            chmod(reciver.c_str(), S_IXUSR);
            ss.close();
            pid_t pid = fork();
            if (pid == 0)
            {
                execve(reciver.c_str(), NULL, NULL);
            }
            else
            {
                exit(0);
            }
        }
        else
        {
            goto restart;
        }
    }
}

void AgentMaster::checkNew()
{
    vector<process> lib_new = check();
    if (!lib_new.empty())
    {
        printf(" NEW PROCESS \n");
        for (int i = 0; i < int(lib_new.size()); ++i)
        {
doAgain:
            pid_t pid = fork();

            if (pid == 0)
            {
                create(lib_new[i]);
                exit(CHILD_STOP);
            }
            else if (pid > 0)
            {
                process p;
                p.p_pid = pid;
                p.p_name = lib_new[i].p_name;
                p.num1 = lib_new[i].num1;
                p.num2 = lib_new[i].num2;
                p.num3 = lib_new[i].num3;
                p.type = lib_new[i].type;
                p.time = lib_new[i].time;
                p.arg = lib_new[i].arg;
                p.status = lib_new[i].status;
                this->listProcess.push_back(p);
                printf(" CREATE NEW %s - %d . %d .%d\n", p.p_name.c_str(), p.num1, p.num2, p.num3);
                return;
            }
            else
            {
                printf("fork fail !\n");
                this->listProcess.pop_back();
                goto doAgain;
            }
        }
    }
}

int AgentMaster::startAll()
{
    for (int i = 0; i < int(listLibs.size()); ++i)
    {
doCreate:
        pid_t pid =fork();
        if (pid == 0)
        {
            create(listLibs[i]);
            return 0;
        }
        else if (pid > 0)
        {
            process p;
            p.p_pid = pid;
            p.p_name = listLibs[i].p_name;
            p.num1 = listLibs[i].num1;
            p.num2 = listLibs[i].num2;
            p.num3 = listLibs[i].num3;
            p.type = listLibs[i].type;
            p.time = listLibs[i].time;
            p.arg = listLibs[i].arg;
            p.status = listLibs[i].status;
            this->listProcess.push_back(p);
        }
        if (pid <0)
        {
            this->listProcess.pop_back();
            goto doCreate;
        }
    }
    return 1;
}

void AgentMaster::checkState()
{
    printState();
    for (int i = 0; i < int(listProcess.size()); ++i)
    {
        if (kill(listProcess[i].p_pid, 0) == -1)
        {
            switch (listProcess[i].type) {
            case 0:
                if (listProcess[i].status == 1)
                {
                    listProcess[i].status = 0;
                    continue;
                }
                else {
                    continue;
                }
                break;
            case 1:
                if (listProcess[i].status == 1)
                {
                    this->listProcess[i].time_death = time(0);
                    this->listProcess[i].status = 0;
                    continue;
                }
                else if (listProcess[i].status == 0) {
                    if (time(0) - listProcess[i].time_death > listProcess[i].time)
                    {
                        this->listProcess[i].status = 1;
                        break;
                    }
                    else {
                        continue;
                    }
                }
                break;
            case 2:
                break;
            default:
                break;
            }
doCreate1:
            pid_t pid = fork();
            if (pid == 0)
            {
                create(listProcess[i]);
                exit(1);
            }
            else if (pid > 0)
            {
                printf("create pid %d  -- %s \n",pid, listProcess[i].p_name.c_str());
                this->listProcess[i].p_pid = pid;
            }
            if (pid <0)
            {
                goto doCreate1;
            }
        }
    }
}

void AgentMaster::update(ZserverClient client)
{
    string empfile = AGENTCHILD_FORDER;
    for (int i = 0; i < int(listProcess.size()); ++i) {
        string new_file = "";
        client.check(new_file,listProcess[i].p_name, listProcess[i].num1, listProcess[i].num2, listProcess[i].num3);
        if (new_file != "")
        {
            printf("---- Update ---- %s \n", new_file.c_str());
            string p_rm1 = listProcess[i].p_name + "-" + to_string(listProcess[i].num1) + "." + to_string(listProcess[i].num2) + "." +to_string(listProcess[i].num3) + ".so";
            printf("     Name of file old: %s   Name of file new: %s\n", p_rm1.c_str(), new_file.c_str());
            pid_t pid = fork();
            if (pid ==0)
            {
                bool g = client.getNewVersion(new_file);
                string p_rm = listProcess[i].p_name + "-" + to_string(listProcess[i].num1) + "." + to_string(listProcess[i].num2) + "." +to_string(listProcess[i].num3) + ".so";
                p_rm = AGENTCHILD_FORDER + p_rm;
                if (g)
                {
                    remove(p_rm.c_str());
                }
                exit(CHILD_STOP);
            }
            else
            {
                sleep(1);
                empfile = empfile + new_file;
                StreamSocket ss;
                ss.connect(SocketAddress("localhost", 9000));
                SocketStream istr(ss);
                std::string file(empfile.c_str());
                Poco::FileOutputStream ostr(file, std::ios::binary);
                StreamCopier::copyStream(istr, ostr);
                printf("send complete \n");
                sleep(1);
                ss.close();

                kill(this->listProcess[i].p_pid, SIGTERM);
                this->listProcess.erase(this->listProcess.begin() + i);
                this->listLibs.erase(this->listLibs.begin() + i);
                checkNew();
                printState();
            }
        }
    }
}

process AgentMaster::checkAgentMaster()
{
    DIR           *dirp;
    struct dirent *directory;
    dirp = opendir(AGENTMASTER_FORDER);
    if (dirp)
    {
        while ((directory = readdir(dirp)) != NULL)
        {
            string str = directory->d_name;
            size_t pos = str.find(".");
            process p;
            if(pos >0)
            {
                int pos1 = str.find("-");
                p.p_name = str.substr(0, pos1);
                if(p.p_name.compare("agentmaster") == 0)
                {
                    int v[3];
                    int k = 0;
                    int count = 0;
                    int old = pos1;
                    char ver[20];
                    for (int i = pos1 + 1; i < int(str.length()); ++i) {
                        count++;
                        if ((str[i] == '.') or (str[i] == '\0'))
                        {
                            size_t length = str.copy(ver, count - 1, old + 1);
                            ver[length]= '\0';
                            string str2(ver);
                            old = i;
                            count = 0;
                            v[k] = atoi(str2.c_str());
                            ++k;
                        }
                    }
                    p.num1 = v[0];
                    p.num2 = v[1];
                    std::size_t length =  str.copy(ver, 1, int(str.size()) - 1);
                    ver[length] = '\0';
                    string str1(ver);
                    p.num3 = atoi(str1.c_str());
                    return p;
                }
            }
        }
    }
    process p1;
    return p1;
}

vector<process> AgentMaster::check()
{
    vector<process> lib_new = scan();
    vector<process> s_both;
    for (int i = 0; i < int(lib_new.size()); ++i)
    {
        bool t_check = true;
        for (int j = 0; j < int(listLibs.size()); ++j)
            if ((lib_new[i].p_name.compare(this->listLibs[j].p_name) == 0))
            {
                t_check = false;
            }
        if (t_check == true)
        {
            process p;
            p.p_name = lib_new[i].p_name;
            p.num1 = lib_new[i].num1;
            p.num2 = lib_new[i].num2;
            p.num3 = lib_new[i].num3;
            p.type = lib_new[i].type;
            p.time = lib_new[i].type;
            p.arg = lib_new[i].arg;
            p.status = lib_new[i].status;
            s_both.push_back(p);
        }
    }
    this->listLibs.insert(listLibs.end(), s_both.begin(), s_both.end());

    return s_both;
}

vector<process> AgentMaster::scan()
{
    DIR           *dirp;
    struct dirent *directory;
    vector<process> listFiles;
    dirp = opendir(AGENTCHILD_FORDER);
    if (dirp)
    {
        while ((directory = readdir(dirp)) != NULL)
        {
            std::string str = directory->d_name;
            std::size_t pos = str.find(".");
            process p;
            if(pos >0)
            {
                int pos1 = str.find("-");
                p.p_name = str.substr(0, pos1);
                int v[3];
                int k = 0;
                int count = 0;
                int old = pos1;
                char ver[20];
                for (int i = pos1 + 1; i < int(str.length()); ++i) {
                    count++;
                    if (str[i] == '.')
                    {
                        std::size_t length = str.copy(ver, count - 1, old + 1);
                        ver[length]= '\0';
                        string str2(ver);
                        old = i;
                        count = 0;
                        v[k] = atoi(str2.c_str());
                        ++k;
                    }
                }
                p.num1 = v[0];
                p.num2 = v[1];
                p.num3 = v[2];

                string config = "config";
                string file = FILE_CONFIG + config + p.p_name + "-" + to_string(p.num1) + "." + to_string(p.num2) + "." + to_string(p.num3) + ".txt";
                FILE* file_config = proc_fopen(file.c_str());

                if (file_config == NULL)
                {
                    printf("Can't open config file \n");
                    return listFiles;
                }

                Parser parser;
                char buffer[8196];
                if (fgets(buffer, sizeof(buffer), file_config)){
                    std::string content = std::string(buffer);
                    Dynamic::Var result = parser.parse(content);
                    Object::Ptr obj = result.extract<Object::Ptr>();
                    int type = obj->get("type");
                    int time = obj->get("time");
                    string arg = obj->get("arg");

                    p.type = type;
                    p.time = time;
                    p.arg = arg;
                    p.status = 1;
                }

                std::size_t length =  str.copy(ver, 2, int(str.size()) - 2);
                ver[length] = '\0';
                string str1(ver);
                if (str1.compare("so") == 0)
                {
                    listFiles.push_back(p);
                }
            }
        }
        closedir(dirp);
    }
    return listFiles;
}

vector<string> AgentMaster::explode(const string &s, const char &c)
{
    string buff{""};
    vector<string> v;

    for(auto n:s)
    {
        if(n != c) buff+=n; else
            if(n == c && buff != "") { v.push_back(buff); buff = ""; }
    }
    if(buff != "") v.push_back(buff);

    return v;
}

int AgentMaster::main(const std::vector<string> &args)
{
    onRun();
    return Application::EXIT_OK;
}


POCO_SERVER_MAIN(AgentMaster)

