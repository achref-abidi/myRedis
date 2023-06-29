//
// @file Connection.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 29 June 2023
// @brief
//
#pragma once

#include "myRedis/Includes.h"
#include <cstring>  ///< For memcpy
#include <unistd.h> ///< For file manipulation.
#include <cassert>  ///< For assert macro.
#include <cstdint>  ///< For fixed width integer types.
#include <string>

// the file descriptor returned by a socket related system call,
// does not provide us with much information on the connection
// so we will be using the struct bellow, to add additional information the connected
// fds.
class Connection
{
public:
    enum
    {
        STATE_REQ = 0, ///< state for reading request.
        STATE_RES = 1, ///< state for sending responses.
        STATE_END = 2  ///< Mark the connection for deletion (in case of errors).
    };

public:
    //
    // @brief Default Constructor
    //
    Connection() = default;

    //
    // Closes the open file descriptor.
    //
    virtual ~Connection();
    //
    // @brief Similar to `read_full` function but it is implemented
    // to be used with non blocking read.
    //
    // @note This function should be called inside a while loop,
    // to safely write all of the data.
    //
    // @return true if there is more data to read.
    //
    bool fillBuffer();

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
    bool flushBuffer();

    //
    // @brief Parses a single request out of the received ones.
    //
    // @note This function should be called inside a while loop
    // to parse all available requests.
    //
    // @return true if there is more requests to parse.
    // @return false if there is no requests to parse or in case of an error.
    //
    bool oneRequest();

    //
    // @brief This will operate on the client depending on its state,
    // i.e read, or write, or delete.
    //
    void connection_io()
    {
        if (this->state == Connection::STATE_REQ)
        {
            while (fillBuffer())
                ;
        }
        else if (this->state == Connection::STATE_RES)
        {
            while (flushBuffer())
                ;
        }
        /*else if (conn.state == Connection::STATE_END)
        {
            // close fd
            close(conn.fd);
            // remove from conn_map
            conn_map.erase(conn.fd);

            // remove from all_fd vector???
        }*/
    }

public:
    int fd = -1;        ///< connection file descriptor.
    uint32_t state = 0; ///< state used to decide what to do with the connection.
    ///< One connection socket is either readable or writable.
    size_t rbuf_size = 0;        ///< size of the reading buffer
    uint8_t rbuf[4 + k_max_msg]; ///< Buffer for reading.
    size_t wbuf_size = 0;        ///< size of the writing buffer
    size_t wbuf_sent = 0;        ///< Size of the sended data.
    uint8_t wbuf[4 + k_max_msg]; ///< Buffer for writing.
};

inline Connection::~Connection()
{
    // we don't close file descriptor only if state == STATE_END
    if (state == STATE_END)
    {
        close(fd);
    }
}
