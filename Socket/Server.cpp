#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <winsock2.h>

using namespace std;

//#pragma comment(lib, "ws2_32.lib")


int main() {
    // ��ʼ�� Winsock
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
            select()�������ṩ��fd_set�����ݽṹ��ʵ������long���͵����飬
            ÿһ������Ԫ�ض�����һ�򿪵��ļ������������socket��������������ļ��������ܵ����豸�����������ϵ��������ϵ�Ĺ����ɳ���Ա���.
            ������select()ʱ�����ں˸���IO״̬�޸�fd_set�����ݣ��ɴ���ִ֪ͨ����select()�Ľ����ĸ�socket���ļ���������˿ɶ����д�¼���
        */
    fd_set rfds;  //���ڼ��socket�Ƿ������ݵ����ĵ��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�������ݵ�����
    fd_set wfds;  //���ڼ��socket�Ƿ���Է��͵��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�����Է������ݣ�
    //bool first_connetion = true;

    //������ַ
    std::string listenAddress;
    //�����˿�
    int listenPort{};
    //��Ŀ¼
    std::string rootDirectory;

    //���������ļ�
    std::ifstream file("config.txt");
    std::string line;
    // ���ж�ȡ�����ļ�
    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;

        // ����������
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

    //��������socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
        exit(1);
    }
    cout << "Socket create Ok!" << endl;



    //���÷������Ķ˿ں͵�ַ
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

    //����
    rtn = listen(serverSocket, 5);
    if (rtn == SOCKET_ERROR)
    {
        std::cerr << "Error listening socket!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    std::cout << "Socket listen OK!" << endl;

    //���ý��ܻ�����
    char recvBuf[4096];

    //��serverSocket����Ϊ������ģʽ�Լ����ͻ�����������
    u_long blockMode = 1;

    //����ioctlsocket,��serverSocket��Ϊ������ģʽ���ĳɷ������fd_setԪ�ص�״̬����ÿ��Ԫ�ض�Ӧ�ľ���Ƿ�ɶ���д
    if ((rtn = ioctlsocket(serverSocket, FIONBIO, &blockMode) == SOCKET_ERROR))
    {
        std::cout << "ioctlsocket() failed with error!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    std::cout << "ioctlsocket() for server socket OK! waiting for client connection" << endl;



    // ���� Winsock
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
//   // ���캯������ʼ��Web�����������������ļ�
//   explicit WebServer(std::string const& configFile);

//   // ����Web������
//   void start();

//  private:
//   // ���������ļ�
//   void loadConfig(std::string const& configFile);

//   // ����ͻ�������
//   void handleRequest(int clientSocket);

//   // ��ȡ�ļ�����
//   static std::string readFile(std::string const& filePath);

//   // ����HTTP��Ӧ����
//   static std::string buildResponse(std::string const& filePath, std::string const& fileContent);

//   // ��ȡ�ļ���MIME����
//   static std::string getContentType(std::string const& filePath);

//   // ��¼������Ϣ
//   static void logRequest(std::string const& ip, int port, std::string const& request);

//   // ��¼��Ӧ��Ϣ
//   static void logResponse(std::string const& result);

//   // ������ַ
//   std::string listenAddress;

//   // �����˿�
//   int listenPort{};

//   // ��Ŀ¼
//   std::string rootDirectory;

//   // �������׽���
//   int serverSocket;
// };

// // ���캯��ʵ��
// WebServer::WebServer(std::string const& configFile) : serverSocket(socket(AF_INET, SOCK_STREAM, 0)) {
//   // ���������ļ�
//   loadConfig(configFile);

//   // �����������׽���
//   if (serverSocket < 0) {
//     std::cerr << "Error creating socket" << std::endl;
//     exit(1);
//   }

//   // ���÷�������ַ�ṹ
//   struct sockaddr_in serverAddr;
//   serverAddr.sin_family      = AF_INET;
//   serverAddr.sin_addr.s_addr = inet_addr(listenAddress.c_str());
//   serverAddr.sin_port        = htons(listenPort);

//   // ���׽���
//   if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
//     std::cerr << "Error binding socket" << std::endl;
//     exit(1);
//   }

//   // �����׽���
//   if (listen(serverSocket, 5) < 0) {
//     std::cerr << "Error listening on socket" << std::endl;
//     exit(1);
//   }
// }

// // ����������
// void WebServer::start() {
//   std::cout << "Server started on " << listenAddress << ":" << listenPort << std::endl;

//   // ����ѭ�����ȴ��ͻ�������
//   while (true) {
//     struct sockaddr_in clientAddr;
//     socklen_t clientAddrLen = sizeof(clientAddr);

//     // ���ܿͻ�������
//     int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
//     if (clientSocket < 0) {
//       std::cerr << "Error accepting connection" << std::endl;
//       continue;
//     }

//     // ����ͻ�������
//     handleRequest(clientSocket);

//     // �رտͻ����׽���
//     close(clientSocket);
//   }
// }

// // ���������ļ�
// void WebServer::loadConfig(std::string const& configFile) {
//   std::ifstream file(configFile);
//   std::string line;

//   // ���ж�ȡ�����ļ�
//   while (std::getline(file, line)) {
//     std::istringstream is_line(line);
//     std::string key;

//     // ����������
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

// // ����ͻ�������
// void WebServer::handleRequest(int clientSocket) {
//   char buffer[1024];

//   // ���տͻ�������
//   ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
//   if (bytesRead < 0) {
//     std::cerr << "Error reading from socket" << std::endl;
//     return;
//   }
//   buffer[bytesRead] = '\0';

//   // ����������
//   std::string request(buffer);
//   std::stringstream requestStream(request);
//   std::string requestLine;
//   std::getline(requestStream, requestLine);

//   std::string method;
//   std::string path;
//   std::string protocol;
//   std::istringstream(requestLine) >> method >> path >> protocol;

//   // �����ļ�·��
//   std::string filePath = rootDirectory + path;
//   if (filePath.back() == '/') {
//     filePath += "index.html";
//   }

//   // ��ȡ�ļ�����
//   std::string fileContent = readFile(filePath);

//   // ������Ӧ����
//   std::string response = buildResponse(filePath, fileContent);

//   // ������Ӧ����
//   send(clientSocket, response.c_str(), response.size(), 0);

//   // ��¼������Ϣ
//   logRequest(inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), requestLine);

//   // ��¼��Ӧ��Ϣ
//   logResponse(fileContent.empty() ? "File not found" : "File served");
// }

// // ��ȡ�ļ�����
// std::string WebServer::readFile(std::string const& filePath) {
//   std::ifstream file(filePath, std::ios::binary);

//   // ����ļ������ڣ����ؿ��ַ���
//   if (!file) {
//     return "";
//   }

//   std::stringstream buffer;
//   buffer << file.rdbuf();
//   return buffer.str();
// }

// // ����HTTP��Ӧ����
// std::string WebServer::buildResponse(std::string const& filePath, std::string const& fileContent) {
//   std::string response;

//   // ����ļ�����Ϊ�գ�����404����
//   if (fileContent.empty()) {
//     response = "HTTP/1.1 404 Not Found\r\n\r\n";
//   } else {
//     // ����200 OK��Ӧ
//     response  = "HTTP/1.1 200 OK\r\n";
//     response += "Content-Type: " + getContentType(filePath) + "\r\n";
//     response += "Content-Length: " + std::to_string(fileContent.size()) + "\r\n";
//     response += "\r\n";
//     response += fileContent;
//   }
//   return response;
// }

// // ��ȡ�ļ���MIME����
// std::string WebServer::getContentType(std::string const& filePath) {
//   std::string extension = fs::path(filePath).extension().string();

//   // �����ļ���չ�����ض�Ӧ��MIME����
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

// // ��¼������Ϣ
// void WebServer::logRequest(std::string const& ip, int port, std::string const& request) {
//   std::cout << "Request from " << ip << ":" << port << " - " << request << std::endl;
// }

// // ��¼��Ӧ��Ϣ
// void WebServer::logResponse(std::string const& result) {
//   std::cout << "Response: " << result << std::endl;
// }

// // ������
// int main() {
//   // ��ʼ�� Winsock
//   WSADATA wsaData;
//   int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
//   if (result != 0) {
//     std::cerr << "WSAStartup failed: " << result << std::endl;
//     return 1;
//   }

//   // ����WebServerʵ��������������
//   WebServer server(serverConfigFile);
//   server.start();

//   // ���� Winsock
//   WSACleanup();

//   return 0;
// }