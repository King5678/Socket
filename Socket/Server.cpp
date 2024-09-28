#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <winsock2.h>

using namespace std;

//#pragma comment(lib, "ws2_32.lib")


int main() {
    // 初始化 Winsock
    WSADATA wsaData;
    int nRc = WSAStartup(WINSOCK_VERSION, &wsaData);

    if (nRc != 0) {
        std::cout << "Winsock  startup failed with error!" << endl;
        WSACleanup();
        exit(1);
    }

    if (wsaData.wVersion != WINSOCK_VERSION) {
        std::cout << "Winsock version is not correct!" << endl;
        WSACleanup();
        exit(1);
    }

    std::cout << "Winsock  startup Ok!" << endl;

    /*
            select()机制中提供的fd_set的数据结构，实际上是long类型的数组，
            每一个数组元素都能与一打开的文件句柄（不管是socket句柄，还是其他文件或命名管道或设备句柄）建立联系，建立联系的工作由程序员完成.
            当调用select()时，由内核根据IO状态修改fd_set的内容，由此来通知执行了select()的进程哪个socket或文件句柄发生了可读或可写事件。
        */
    fd_set rfds;  //用于检查socket是否有数据到来的的文件描述符，用于socket非阻塞模式下等待网络事件通知（有数据到来）
    fd_set wfds;  //用于检查socket是否可以发送的文件描述符，用于socket非阻塞模式下等待网络事件通知（可以发送数据）
    //bool first_connetion = true;

    //监听地址
    std::string listenAddress;
    //监听端口
    int listenPort{};
    //主目录
    std::string rootDirectory;

    //加载配置文件
    std::ifstream file("config.txt");
    std::string line;
    // 逐行读取配置文件
    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;

        // 解析配置项
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                if (key == "ListenAddress") {
                    listenAddress = value;
                }
                else if (key == "ListenPort") {
                    listenPort = std::stoi(value);
                }
                else if (key == "RootDirectory") {
                    rootDirectory = value;
                }
            }
        }
    }

    //创建监听socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
        exit(1);
    }
    cout << "Socket create Ok!" << endl;



    //设置服务器的端口和地址
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(listenPort);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(listenAddress.c_str());

    //binding
    int rtn = bind(serverSocket, (LPSOCKADDR)&serverAddr, sizeof(serverAddr));
    if (rtn == SOCKET_ERROR)
    {
        std::cerr << "Error binding socket!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    std::cout << "Socket bind OK!" << endl;

    //监听
    rtn = listen(serverSocket, 5);
    if (rtn == SOCKET_ERROR)
    {
        std::cerr << "Error listening socket!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    std::cout << "Socket listen OK!" << endl;

    //设置接受缓冲区
    char recvBuf[4096];

    //将serverSocket设置为非阻塞模式以监听客户端连接请求
    u_long blockMode = 1;

    //调用ioctlsocket,将serverSocket改为非阻塞模式，改成反复检查fd_set元素的状态，看每个元素对应的句柄是否可读可写
    if ((rtn = ioctlsocket(serverSocket, FIONBIO, &blockMode) == SOCKET_ERROR))
    {
        std::cout << "ioctlsocket() failed with error!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    std::cout << "ioctlsocket() for server socket OK! waiting for client connection" << endl;



    // 清理 Winsock
    WSACleanup();
}

// #include <filesystem>
// #include <fstream>
// #include <iostream>
// #include <sstream>
// #include <string>

// #include <fcntl.h>
// #include <unistd.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>

// namespace fs = std::filesystem;

// std::string const serverConfigFile = "config.txt";

// class WebServer {
//  public:
//   // 构造函数，初始化Web服务器并加载配置文件
//   explicit WebServer(std::string const& configFile);

//   // 启动Web服务器
//   void start();

//  private:
//   // 加载配置文件
//   void loadConfig(std::string const& configFile);

//   // 处理客户端请求
//   void handleRequest(int clientSocket);

//   // 读取文件内容
//   static std::string readFile(std::string const& filePath);

//   // 构建HTTP响应报文
//   static std::string buildResponse(std::string const& filePath, std::string const& fileContent);

//   // 获取文件的MIME类型
//   static std::string getContentType(std::string const& filePath);

//   // 记录请求信息
//   static void logRequest(std::string const& ip, int port, std::string const& request);

//   // 记录响应信息
//   static void logResponse(std::string const& result);

//   // 监听地址
//   std::string listenAddress;

//   // 监听端口
//   int listenPort{};

//   // 主目录
//   std::string rootDirectory;

//   // 服务器套接字
//   int serverSocket;
// };

// // 构造函数实现
// WebServer::WebServer(std::string const& configFile) : serverSocket(socket(AF_INET, SOCK_STREAM, 0)) {
//   // 加载配置文件
//   loadConfig(configFile);

//   // 创建服务器套接字
//   if (serverSocket < 0) {
//     std::cerr << "Error creating socket" << std::endl;
//     exit(1);
//   }

//   // 设置服务器地址结构
//   struct sockaddr_in serverAddr;
//   serverAddr.sin_family      = AF_INET;
//   serverAddr.sin_addr.s_addr = inet_addr(listenAddress.c_str());
//   serverAddr.sin_port        = htons(listenPort);

//   // 绑定套接字
//   if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
//     std::cerr << "Error binding socket" << std::endl;
//     exit(1);
//   }

//   // 监听套接字
//   if (listen(serverSocket, 5) < 0) {
//     std::cerr << "Error listening on socket" << std::endl;
//     exit(1);
//   }
// }

// // 启动服务器
// void WebServer::start() {
//   std::cout << "Server started on " << listenAddress << ":" << listenPort << std::endl;

//   // 无限循环，等待客户端连接
//   while (true) {
//     struct sockaddr_in clientAddr;
//     socklen_t clientAddrLen = sizeof(clientAddr);

//     // 接受客户端连接
//     int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
//     if (clientSocket < 0) {
//       std::cerr << "Error accepting connection" << std::endl;
//       continue;
//     }

//     // 处理客户端请求
//     handleRequest(clientSocket);

//     // 关闭客户端套接字
//     close(clientSocket);
//   }
// }

// // 加载配置文件
// void WebServer::loadConfig(std::string const& configFile) {
//   std::ifstream file(configFile);
//   std::string line;

//   // 逐行读取配置文件
//   while (std::getline(file, line)) {
//     std::istringstream is_line(line);
//     std::string key;

//     // 解析配置项
//     if (std::getline(is_line, key, '=')) {
//       std::string value;
//       if (std::getline(is_line, value)) {
//         if (key == "ListenAddress") {
//           listenAddress = value;
//         } else if (key == "ListenPort") {
//           listenPort = std::stoi(value);
//         } else if (key == "RootDirectory") {
//           rootDirectory = value;
//         }
//       }
//     }
//   }
// }

// // 处理客户端请求
// void WebServer::handleRequest(int clientSocket) {
//   char buffer[1024];

//   // 接收客户端请求
//   ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
//   if (bytesRead < 0) {
//     std::cerr << "Error reading from socket" << std::endl;
//     return;
//   }
//   buffer[bytesRead] = '\0';

//   // 解析请求报文
//   std::string request(buffer);
//   std::stringstream requestStream(request);
//   std::string requestLine;
//   std::getline(requestStream, requestLine);

//   std::string method;
//   std::string path;
//   std::string protocol;
//   std::istringstream(requestLine) >> method >> path >> protocol;

//   // 构建文件路径
//   std::string filePath = rootDirectory + path;
//   if (filePath.back() == '/') {
//     filePath += "index.html";
//   }

//   // 读取文件内容
//   std::string fileContent = readFile(filePath);

//   // 构建响应报文
//   std::string response = buildResponse(filePath, fileContent);

//   // 发送响应报文
//   send(clientSocket, response.c_str(), response.size(), 0);

//   // 记录请求信息
//   logRequest(inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), requestLine);

//   // 记录响应信息
//   logResponse(fileContent.empty() ? "File not found" : "File served");
// }

// // 读取文件内容
// std::string WebServer::readFile(std::string const& filePath) {
//   std::ifstream file(filePath, std::ios::binary);

//   // 如果文件不存在，返回空字符串
//   if (!file) {
//     return "";
//   }

//   std::stringstream buffer;
//   buffer << file.rdbuf();
//   return buffer.str();
// }

// // 构建HTTP响应报文
// std::string WebServer::buildResponse(std::string const& filePath, std::string const& fileContent) {
//   std::string response;

//   // 如果文件内容为空，返回404错误
//   if (fileContent.empty()) {
//     response = "HTTP/1.1 404 Not Found\r\n\r\n";
//   } else {
//     // 构建200 OK响应
//     response  = "HTTP/1.1 200 OK\r\n";
//     response += "Content-Type: " + getContentType(filePath) + "\r\n";
//     response += "Content-Length: " + std::to_string(fileContent.size()) + "\r\n";
//     response += "\r\n";
//     response += fileContent;
//   }
//   return response;
// }

// // 获取文件的MIME类型
// std::string WebServer::getContentType(std::string const& filePath) {
//   std::string extension = fs::path(filePath).extension().string();

//   // 根据文件扩展名返回对应的MIME类型
//   if (extension == ".html") {
//     return "text/html";
//   }
//   if (extension == ".jpg" || extension == ".jpeg") {
//     return "image/jpeg";
//   }
//   if (extension == ".png") {
//     return "image/png";
//   }
//   if (extension == ".gif") {
//     return "image/gif";
//   }
//   if (extension == ".css") {
//     return "text/css";
//   }
//   if (extension == ".js") {
//     return "application/javascript";
//   }
//   return "text/plain";
// }

// // 记录请求信息
// void WebServer::logRequest(std::string const& ip, int port, std::string const& request) {
//   std::cout << "Request from " << ip << ":" << port << " - " << request << std::endl;
// }

// // 记录响应信息
// void WebServer::logResponse(std::string const& result) {
//   std::cout << "Response: " << result << std::endl;
// }

// // 主函数
// int main() {
//   // 初始化 Winsock
//   WSADATA wsaData;
//   int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
//   if (result != 0) {
//     std::cerr << "WSAStartup failed: " << result << std::endl;
//     return 1;
//   }

//   // 创建WebServer实例并启动服务器
//   WebServer server(serverConfigFile);
//   server.start();

//   // 清理 Winsock
//   WSACleanup();

//   return 0;
// }