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

    uint32_t err = query(fd, "Hello1");
    if (err)
        std::cout << "error\n";
    err = query(fd, "Hello2");
    if (err)
        std::cout << "error\n";

    err = query(fd, "Hello3");
    if (err)
        std::cout << "error\n";

    close(fd);
}

int32_t query(int fd, const char *text)
{
    uint32_t len = (uint32_t)strlen(text);
    if (len > k_max_msg)
    {
        return -1;
    }

    // Constructing the message frame according to our protocol
    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4); // assume little endian
    memcpy(&wbuf[4], text, len);

    // sending the message
    if (int32_t err = write_full(fd, wbuf, 4 + len))
    {
        return err;
    }

    // Reading server response
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err)
    {
        if (errno == 0)
        {
            std::cout << "EOF\n";
        }
        else
        {
            std::cout << "Error while reading\n";
        }
        return err;
    }

    memcpy(&len, rbuf, 4); // assume little endian
    if (len > k_max_msg)
    {
        std::cout << "Too long\n";
        return -1;
    }

    err = read_full(fd, &rbuf[4], len);
    if (err)
    {
        std::cout << "Read error\n";
    }

    rbuf[4 + len] = '\0';
    std::cout << "Server says: " << &rbuf[4] << std::endl;

    return 0;
}