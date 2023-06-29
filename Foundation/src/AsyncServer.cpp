//
// @file ServerWithEventLoop.cpp
// @headerfile Server.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 25 June 2023
// @brief Holds the definition of `server()` function and it's helper functions.
// The implementation of different functions are done to be used
// in an event loop.
//
#include "myRedis/Servcer.h"

#ifdef EVENT_LOOP

void Server()
{
    // Setting up listening fd
    int listening_fd = setup_server();

    // A map of all client connections, keyed by fd.
    connection_map conn_map;

    // set the listen fd to non blocking mode
    fd_set_nb(listening_fd);

    //
    // The event loop
    //
    std::vector<pollfd> all_fds;
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
            pfd.events = (conn.state == STATE_REQ) ? POLLIN : POLLOUT;
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
                    accept_new_connection(pfd.fd, conn_map);
                }
                else
                {
                    auto &conn = conn_map.at(pfd.fd);

                    // read or write from an active fd
                    connection_io(conn, conn_map);
                }
            }
        }
    } // event loop

    return;
}

void connection_io(Conn &conn, connection_map &conn_map)
{
    if (conn.state == STATE_REQ)
    {
        while (try_fill_buffer(conn))
            ;
    }
    else if (conn.state == STATE_RES)
    {
        while (try_flush_buffer(conn))
            ;
    }
    else if (conn.state == STATE_END)
    {
        // close fd
        close(conn.fd);
        // remove from conn_map
        conn_map.erase(conn.fd);

        // remove from all_fd vector???
    }
}

void accept_new_connection(int fd, connection_map &conn_map)
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
    fd_set_nb(fd);

    // creating a Conn object
    Conn connection;
    connection.fd = connFd;
    connection.state = STATE_REQ;
    connection.rbuf_size = 0;
    connection.wbuf_size = 0;
    connection.wbuf_sent = 0;

    conn_map.insert(std::make_pair(connection.fd, connection));

    // Normally we should add the connected fd to the array that we passed to
    // `poll` system call
    // TODO: why don't we do that here?
}

bool try_fill_buffer(Conn &conn)
{
    // conn.rbuf_size will mark the currently read bytes.
    // i.e it will serve as an index.
    auto &end_of_rbuf = conn.rbuf_size;   // i.e actual len of rbuf
    auto len_of_rbuf = sizeof(conn.rbuf); // Fixed from the start.

    assert(end_of_rbuf < len_of_rbuf); // make sure that ???

    ssize_t retval = 0;
    do
    {
        size_t remainingBytes = len_of_rbuf - end_of_rbuf;
        retval = read(conn.fd, conn.rbuf + end_of_rbuf, remainingBytes);
        /// @note If read() has not read any data yet, it returns -1 and sets errno to EINTR
    } while (retval < 0 && errno == EINTR);
    // We loop over `read()` system call until
    // relval >= 0 or
    // when error has occured due to EAGAIN

    //
    // Handling retval
    //
    if (retval < 0 && errno == EAGAIN)
    {
        // If some process has the socket open for writing and
        // O_NONBLOCK is set to 1, read() returns -1 and
        // sets errno to EAGAIN.
        return false;
    }
    // else if not EAGAIN ... (other errors)
    if (retval < 0)
    {
        std::cout << "read() errro" << std::endl;
        conn.state = STATE_END;
        return false;
    }
    // else if zero bytes are been read ...
    if (retval == 0)
    {
        if (end_of_rbuf > 0)
        {
            std::cout << "Unexpected EOF" << std::endl;
        }
        else
        {
            std::cout << "EOF" << std::endl;
        }
        conn.state = STATE_END;
        return false;
    }

    //
    // ELSE retval > 0
    //

    // Incrementing the end of rbuf for the next read
    end_of_rbuf += (size_t)retval;

    assert(end_of_rbuf <= len_of_rbuf - end_of_rbuf);

    // since the received message can have multiple
    // requests (pipelining).
    while (try_one_request(conn))
        ;
    /// @note in contrast to read_full we need to process the
    /// the received data as soon as we read it.
    // in order to free some space in the rbuff.

    return (conn.state == STATE_REQ);
    // this function is intented to be called inside a loop
    // as long as it is possible to read new data
    // i.e the state of the connection should permit it
}

bool try_one_request(Conn &conn)
{
    // parsing the len of the data
    if (conn.rbuf_size < 4)
        return false;

    int32_t len;
    memcpy(&len, conn.rbuf, 4);

    // reading len bytes of data
    if (conn.rbuf_size < len)
        return false;
    // no suffisiant data is read

    if (len > k_max_msg)
    {
        std::cout << "Data is too long" << std::endl;
        // return true;
    }

    uint8_t a_request[k_max_msg];
    memcpy(a_request, conn.rbuf + 4, len);

    // do something with the request
    a_request[len] = '\0';
    std::cout << a_request << std::endl;

    // sort of freeing those 4 bytes that have been read
    auto remain = conn.rbuf_size - len - 4;
    if (remain > 0)
    {
        memmove(conn.rbuf, conn.rbuf + 4 + len, remain);
    }
    conn.rbuf_size = remain;

    // Reply to the request by inserting an echo message
    memcpy(conn.wbuf + conn.wbuf_size, &len, 4);
    memcpy(conn.wbuf + conn.wbuf_size + 4, a_request, len);
    conn.wbuf_size += 4 + len;

    conn.state = STATE_RES;
    // after reading the request we need to send a response
    //

    while (try_flush_buffer(conn))
        ;

    return conn.state == STATE_REQ; // Return to the outer loop when the
    // response is done processing
}

bool try_flush_buffer(Conn &conn)
{
    ssize_t retval = 0;
    do
    {
        size_t remainingBytes = conn.wbuf_size - conn.wbuf_sent;
        retval = write(conn.fd, conn.wbuf + conn.wbuf_sent, remainingBytes);
    } while (retval < 0 && errno == EINTR);

    //
    // Handling error
    //
    if (retval < 0 && errno == EAGAIN)
    {
        return false;
    }

    if (retval < 0)
    {
        std::cout << "Unexpected error" << std::endl;
        conn.state = STATE_END;
        return false;
    }

    //
    // ELSE retval >= 0
    //
    conn.wbuf_sent += retval;
    assert(conn.wbuf_sent <= conn.wbuf_size);

    if (conn.wbuf_sent == conn.wbuf_size)
    {
        std::cout << "No more bytes to write" << std::endl;
        conn.state = STATE_REQ; // switch back to request state => the conn is readable
        conn.wbuf_sent = 0;
        conn.wbuf_size = 0;
        return false;
    }

    // still more data to write
    return true;
}

void fd_set_nb(int socket_fd)
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
#endif