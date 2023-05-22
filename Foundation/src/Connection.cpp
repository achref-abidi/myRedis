//
// @file Connection.cpp
// @headerfile Connection.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 21 May 2023
// @brief
//
#include "myRedis/Connection.h"

void Connection()
{
    //
    // Socket
    //
    int fd = socket(AF_INET, SOCK_STREAM, (int)0);
    if (fd == -1)
    {
        std::cout << "error\n";
    }

    //
    // connect
    //
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    int rv = connect(fd, (const sockaddr *)&addr, sizeof((addr)));
    if (rv)
    {
        std::cout << "errro\n";
    }

    //
    // write
    //
    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    //
    // read
    //
    char readBuffer[64];
    ssize_t n = read(fd, readBuffer, sizeof(readBuffer) - 1);
    if (n < 0)
    {
        std::cout << "error\n";
        return;
    }

    std::cout << "Server says " << readBuffer << std::endl;

    close(fd);
}