#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

int main()
{
    int serviceSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serviceSock == -1)
    {
        std::cout << "error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in serviceAddr;
    memset(&serviceAddr, 0, sizeof(sockaddr_in));
    serviceAddr.sin_family = AF_INET; //ipv4
    serviceAddr.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0
    serviceAddr.sin_port = htons(8080); // 8080端口

    int flag = 1;
    setsockopt(serviceSock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    int ret = bind(serviceSock, (struct sockaddr*)&serviceAddr, sizeof(serviceAddr));
    if (ret == -1)
    {
        std::cout << "error binding socket" << std::endl;
        close(serviceSock);
        return 1;
    }

    ret = listen(serviceSock, 5);
    if (ret == -1)
    {
        std::cout << "error listening socket" << std::endl;
        close(serviceSock);
        return 1;
    }

    std::cout << "Server is running on port 8080..." << std::endl;

    //加入while循环 可以让服务器一直接受客户端的请求
    while (true)
    {
        struct sockaddr_in clientAddr;
        memset(&clientAddr, 0 , sizeof(clientAddr));
        socklen_t clientAddrLen = sizeof(clientAddr);
    
        int clientSock = accept(serviceSock, (struct sockaddr*)&clientAddr, &clientAddrLen);
    
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, INET_ADDRSTRLEN);
        unsigned short clientPort = ntohs(clientAddr.sin_port);
        std::cout << "ClientInfo: " << clientIP << ":" << clientPort << std::endl;
    
        char buff[1024];
        size_t byteRead = recv(clientSock, buff, sizeof(buff), 0);
        if (byteRead < 0)
        {
            std::cout << "error reading from socket" << std::endl;
            close(serviceSock);
            close(clientSock);
            return 1;
        }
        std::string request(buff, byteRead);
        std::cout << "Received:\n" << request <<std::endl;
    
    
        //构造响应报文
        std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "Hello, World!";
    
        //发送HTTP响应
        ret = write(clientSock, response.c_str(), response.length());
        if (ret == -1)
        {
            std::cout << "error write response" << std::endl;
        }
        close(clientSock);   
 
    }
    close(serviceSock); //问题：seviceSock不会被释放
}