//
// @file Server.cpp
// @headerfile Server.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 21 May 2023
// @brief
//
#include "myRedis/Servcer.h"

void Server()
{

    //
    // Creating socket
    //
    int fd = socket(AF_INET, SOCK_STREAM, (int)0);
    if (fd == -1)
        std::cout << "error\n";

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    //
    // bind, this is the syntax that deals with IPv4.
    //
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        std::cout << "error\n";
    }

    //
    // Listening
    //
    rv = listen(fd, SOMAXCONN);
    if (rv)
    {
        std::cout << "error\n";
    }

    //
    // Loop for every connection
    //
    while (true)
    {
        struct sockaddr_in clientAddr = {};
        socklen_t clientAddrLen = sizeof(clientAddr);

        int connFd = accept(fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (connFd < 0)
        {
            continue; // error
        }

        do_something(connFd);
        close(connFd);
    }
}

void do_something(int connectionFileDescriptor)
{
    char readBuffer[64];
    ssize_t n = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer) - 1);
    if (n < 0)
    {
        std::cout << "error\n";
        return;
    }

    std::cout << "Client says " << readBuffer << std::endl;

    char writeBuffer[] = "world";
    write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
}
