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
#include <poll.h>       ///< For `poll()` system call.
#include <fcntl.h>      ///< For fcntl function
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>

const size_t k_max_msg = 4096; ///< Equals to 4 KBytes.

#ifdef EVENT_LOOP
using pollfd = struct pollfd;

enum
{
    STATE_REQ = 0, ///< state for reading request.
    STATE_RES = 1, ///< state for sending responses.
    STATE_END = 2  ///< Mark the connection for deletion (in case of errors).
};
// the file descriptor returned by a socket related system call,
// does not provide us with much information on the connection
// so we will be using the struct bellow, to add additional information the connected
// fds.
struct Conn
{
    int fd = -1;        ///< connection file descriptor.
    uint32_t state = 0; ///< state used to decide what to do with the connection.
    ///< One connection socket is either readable or writable.
    size_t rbuf_size = 0;        ///< size of the reading buffer
    uint8_t rbuf[4 + k_max_msg]; ///< Buffer for reading.
    size_t wbuf_size = 0;        ///< size of the writing buffer
    size_t wbuf_sent = 0;        ///< Size of the sended data.
    uint8_t wbuf[4 + k_max_msg]; ///< Buffer for writing.
};

//
// @brief A map of all client connections, keyed by fd.
//
using connection_map = std::unordered_map<int, Conn>;

//
// @brief To set read(), write(), accpec() to a non blocking mode.
//
void fd_set_nb(int listen_fd);

//
// @brief This will operate on the client depending on its state,
/// i.e read, or write, or delete.
//
void connection_io(Conn &conn_fd, connection_map &conn_map);

//
// @brief Accept a new connection an add it to the
// connection_map
//
void accept_new_connection(int fd, connection_map &conn_map);

//
// @brief Similar to `read_full` function but it is implemented
// to be used with non blocking read.
//
// @note This function should be called inside a while loop,
// to safely write all of the data.
//
// @return true if there is more data to read.
//
bool try_fill_buffer(Conn &conn);

//
// @brief Similar to `write_full` function but it is implemented
// to be used with non blocking write.
//
// @note This function should be called inside a while loop,
// to safely write all of the data.
//
// @return true if there is more data to write;
// @return false if there is no more data to write or an error occured.
//
bool try_flush_buffer(Conn &conn);

//
// @brief To parse a single request
// according to our protocol
//
bool try_one_request(Conn &conn);

#endif

//
// @brief Main function for the server
//
void Server();

//
// @brief A helper function that sets up the server
//
int setup_server();

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
// @note This function is decapretated.
//
void do_something(int connectionFileDescriptor);
