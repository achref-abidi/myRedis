//
// @file Connection.h
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

#include "Servcer.h"

void Connection();

//
// @brief Function for making requests and receiving responses.
//
int32_t query(int fd, const char *text);