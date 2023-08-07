//
// @file Client.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 21 May 2023
// @brief
//
#pragma once

#include <sys/socket.h> ///< For network related system calls.
#include <netinet/in.h> ///< For `sockaddr_in` struct.
#include <unistd.h>     ///< For file manipulation.
#include <iostream>
#include <cstring>

#include "Server.h"
#include "Core.h"

/**
 * How to use:
 * Application app = new Application();
 * app.initConnection();
 * app.get(), set(), del()
 * app.closeConnection();
 */

class Application{
public:
    Application() = default;

    virtual ~Application() = default;

    bool initConnection();

    bool closeConnection();

    //bool echo(const std::string& msg, myRedis::str::Response& res);

    bool get(const std::string& key, myRedis::str::Response& res);

    bool set(const std::string& key, const std::string& value, myRedis::str::Response& res);

    bool del(const std::string& key, myRedis::str::Response& res);

protected:
    //
    // @brief Function for making requests and receiving responses.
    //
    bool query(const std::vector<std::string>& q, myRedis::str::Response& res, int nbr_cmd = 1);

private:
    int m_fd = -1; ///< socket descriptor

};