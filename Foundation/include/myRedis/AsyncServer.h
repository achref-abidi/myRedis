//
// @file AsyncServer.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 29 June 2023
//
#pragma once

#include <sys/socket.h> ///< For network related system calls.
#include <netinet/in.h> ///< For `sockaddr_in` struct.
#include <poll.h>       ///< For `poll()` system call.
#include <fcntl.h>      ///< For fcntl function
#include <unordered_map>
#include <vector>
#include <iostream>
#include "myRedis/Connection.h"

using Conn = Connection;
//
// @brief A map of all client connections, keyed by fd.
//
using connection_map = std::unordered_map<int, Conn>;
using pollfd = struct pollfd;

// @brief Holds the definition of `server()` function and it's helper functions.
// The implementation of different functions are done to be used
// in an event loop.
class AsyncServer
{

public:
    AsyncServer();
    ~AsyncServer();

    //
    // @brief Implementation of the event loop.
    //
    void run();

    //
    // @brief To set read(), write(), accpet() to a non blocking mode.
    //
    static void fd_set_nb(int socket_fd);

    static int setup_server();

    //
    // @brief Accept a new connection an add it to the
    // connection_map
    //
    void accept_new_connection(int fd);

private:
    int listening_fd = 0;
    connection_map conn_map;
    std::vector<pollfd> all_fds;
};

inline AsyncServer::~AsyncServer()
{
    close(listening_fd);
    /// TODO: free other fds
}
