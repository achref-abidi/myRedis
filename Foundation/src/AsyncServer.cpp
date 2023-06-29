//
// @file AsyncServer.cpp
// @headerfile AsyncServer.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 25 June 2023
//
#include "myRedis/AsyncServer.h"

AsyncServer::AsyncServer()
{
    // Setting up listening fd
    listening_fd = setup_server();

    // set the listen fd to non blocking mode
    AsyncServer::fd_set_nb(listening_fd);
}

void AsyncServer::run()
{
    while (true)
    {
        //
        // Re-initialize the `poll` args
        //
        all_fds.clear();
        // For convenience, the listening fd is put in the first position.
        struct pollfd pfd = {listening_fd, POLLIN, 0};
        all_fds.push_back(pfd);

        // Then we push back the connected fds
        for (const auto &kv : conn_map)
        {
            auto conn = kv.second;
            // creating the pollfd struct
            struct pollfd pfd = {};
            pfd.fd = conn.fd;
            pfd.events = (conn.state == Connection::STATE_REQ) ? POLLIN : POLLOUT;
            pfd.revents = pfd.events | POLLERR; // the pfd is set to be inactive

            // pushing it.
            all_fds.push_back(pfd);
        }

        //
        // Poll for active fds
        //
        int rv = poll(all_fds.data(), (nfds_t)all_fds.size(), 1000);
        if (rv < 0)
        {
            std::cout << "error\n";
        }

        //
        // Process active fds
        //
        for (pollfd &pfd : all_fds)
        {
            // selecting active fds
            // @note at first time it will select the listening fd that has been pushed first.
            if (pfd.revents)
            {
                if (pfd.fd == listening_fd)
                {
                    // when listening_fd is active i.e. it is ready to accept new connection.
                    accept_new_connection(pfd.fd);
                }
                else
                {
                    auto &conn = conn_map.at(pfd.fd);

                    // read or write from an active fd
                    conn.connection_io();

                    if (conn.state == Connection::STATE_END)
                    {
                        // remove from conn_map
                        conn_map.erase(conn.fd);

                        // TODO: should close fd

                        // remove from all_fd vector???
                    }
                }
            }
        }
    } // event loop

    return;
}

void AsyncServer::accept_new_connection(int fd)
{
    //
    // accept
    //
    struct sockaddr_in clientAddr = {};
    socklen_t clientAddrLen = sizeof(clientAddr);

    int connFd = accept(fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (connFd < 0)
    {
        std::cout << "error accept()" << std::endl;
        return; // error
    }

    // set the fd to non blocking mode
    AsyncServer::fd_set_nb(fd);

    // creating a Conn object
    Conn connection;
    connection.fd = connFd;
    connection.state = Connection::STATE_REQ;
    connection.rbuf_size = 0;
    connection.wbuf_size = 0;
    connection.wbuf_sent = 0;

    conn_map.insert(std::make_pair(connection.fd, connection));

    // Normally we should add the connected fd to the array that we passed to
    // `poll` system call
    // TODO: why don't we do that here?
}

void AsyncServer::fd_set_nb(int socket_fd)
{
    errno = 0;
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (errno)
    {
        exit(1);
    }

    errno = 0;
    (void)fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

    if (errno)
    {
        exit(1);
    }
}

int AsyncServer::setup_server()
{
    //
    // Creating socket
    //
    int fd = socket(AF_INET, SOCK_STREAM, (int)0);
    if (fd == -1)
    {
        LOG("Error while creating socket");
    }

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

    return fd;
}