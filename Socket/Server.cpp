#include <iostream>
#include <winsock2.h>

using namespace std;

//监听地址
std::string listenAddress;
//监听端口
int listenPort{};
//套接字
int serverSocket;
//主目录
std::string rootDirectory;



void loadConfig(const string& configFile)
{

}



int main()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    loadConfig("config.txt");

    if (serverSocket < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(listenAddress.c_str());

}

