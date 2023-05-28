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
        //
        // accept
        //
        struct sockaddr_in clientAddr = {};
        socklen_t clientAddrLen = sizeof(clientAddr);

        int connFd = accept(fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (connFd < 0)
        {
            continue; // error
        }

        // Only server one client at once (no event loop is implemented yet)
        // that means, after the first client is done, we need to restart the server
        // so it can accept another one.
        while (true)
        {
            int32_t err = one_request(connFd);
            if (err)
            {
                break;
            }
        }

        // do_something(connFd);
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

int32_t read_full(int connFd, char *buff, int n)
{
    //
    // To properly handle reading from sockets.
    // More on how `read()` system call behave for sockets:
    // https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-read-read-from-file-socket
    //

    while (n > 0)
    {
        ssize_t rv = read(connFd, buff, n);
        if (rv < 0)
            return rv;

        assert((size_t)rv <= n); // program will block if (!condition)

        n -= (size_t)rv;
        buff += rv;
        // The above line is a little bit tricky, as we passed buf by reference i.e pointer
        // but the actual pointer is passed by copy, so incrementing it won't change anything.
        // => the pointer itself is copied!
    }

    return 0; // reached EOF.
}

int32_t write_full(int connFd, char *buff, int n)
{
    //
    // To properly handle reading from sockets.
    // More on how `write()` system call behave for sockets:
    // https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-write-write-data-file-socket
    //

    while (n > 0)
    {
        ssize_t rv = write(connFd, buff, n);
        if (rv < 0)
        {
            return rv;
        }

        assert((size_t)rv <= n);

        n -= (size_t)rv;
        buff += rv;
    }

    return 0;
}

int32_t one_request(int connFd)
{
    //
    // Since stream sockets acts like streams of information with no boundaries
    // seperating data, we need to implement some sort of protocol to seperate
    // each request from the others.
    //

    char rbuf[4 + k_max_msg + 1]; // 4 bytes header

    errno = 0;

    int32_t err = read_full(connFd, rbuf, 4); // reading the header first i.e the actual length of data
    if (err)
    {
        if (errno == 0)
        {
            std::cout << "EOF\n";
        }
        else
        {
            std::cout << "error while reading\n";
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4); // assume little endian
    if (len > k_max_msg)
    {
        std::cout << "Message is too long\n";
        return -1;
    }

    // request body
    err = read_full(connFd, rbuf, len);

    if (err)
    {
        std::cout << "error while reading\n";
        return err;
    }

    // do something with received data
    rbuf[4 + len] = '\0';
    std::cout << rbuf << std::endl;

    // reply using the same protocol
    const char reply[] = "World";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);

    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);

    return write_full(connFd, wbuf, 4 + len);
}