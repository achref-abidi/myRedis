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
#include <cassert>      ///< For assert macro.
#include <iostream>
#include <string>
#include <cstring>

const size_t k_max_msg = 4096; ///< Equals to 4 KBytes.

void Server();

//
// @brief Parse a single request and replies.
// for this we made up our own communication protocol in order
// to split requests apart.
//
// @param connFd socket file descriptor.
// @return error code
//
int32_t one_request(int connFd);

//
// Helper Functions
//

//
// @brief reads n bytes from the kernel.
//
// @param connFd socket connection file descriptor.
// @param buff a string to store the result of reading.
// @param n number of bytes to read.
//
int32_t read_full(int connFd, char *buff, int n);

//
// @brief Writes n bytes of data
//
// @param connFd socket connection file descriptor.
// @param buff data to be written.
// @param n number of bytes to write.
//
int32_t write_full(int connFd, char *buff, int n);

//
// @brief Just a simple function to manipulate a received response
//
void do_something(int connectionFileDescriptor);
