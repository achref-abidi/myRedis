//
// @file Server.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 21 May 2023
// @brief
//
#pragma once

#include <sys/socket.h> ///< For network related system calls.
#include <netinet/in.h> ///< For `sockaddr_in` struct.
#include <unistd.h>     ///< For file manipulation.
#include <iostream>
#include <string>
#include <cstring>

void Server();
void do_something(int connectionFileDescriptor);