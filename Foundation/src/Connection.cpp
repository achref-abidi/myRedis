//
// @file Connection.cpp
// @headerfile Connection.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 29 June 2023
// @brief
//
#include "myRedis/Connection.h"

bool Connection::oneRequest()
{
    // parsing the len of the data
    //if (this->rbuf_size < 4)
    //    return false;

    //int32_t len;
    //memcpy(&len, this->rbuf, 4);

    // reading len bytes of data
    //if (this->rbuf_size < len)
    //    return false;
    // no suffisiant data is read

    //if (len > k_max_msg)
    //{
    //    LOG("[INFO] The received data is too long");
        // return true;
    //}

    if(this->rbuf_size == 0) return false;

    uint8_t a_request[myRedis::k_max_msg];
    memcpy(a_request, this->rbuf, this->rbuf_size);

    // sort of freeing those bytes that have been read
    //auto remain = this->rbuf_size - len - 4;
    //if (remain > 0)
    //{
    //    memmove(this->rbuf, this->rbuf + 4 + len, remain);
    //}
    //this->rbuf_size = remain;



    // do something with the request
    a_request[this->rbuf_size] = '\0';
    LOG("[INFO] Received message : " + std::string((char *)a_request)); ///TODO: logs should handle \0 char

    parse_commands(a_request, this->rbuf_size);

    // Free the buffer by just setting size to 0
    this->rbuf_size = 0;

    this->state = STATE_RES;
    // after reading the request we need to send a response

    LOG("[INFO] Sending response...");
    while (this->flushBuffer())
        ;

    return this->state == STATE_REQ; // Return to the outer loop when the
    // response is done processing
}

bool Connection::fillBuffer()
{
    LOG("[INFO] attempting to read from socket fd.");

    // conn.rbuf_size will mark the currently read bytes.
    // i.e it will serve as an index.
    auto &end_of_rbuf = this->rbuf_size;   // i.e actual len of rbuf
    auto len_of_rbuf = sizeof(this->rbuf); // Fixed from the start.

    assert(end_of_rbuf < len_of_rbuf); // make sure that rbuf is not full

    ssize_t retval = 0;
    do
    {
        size_t remainingBytes = len_of_rbuf - end_of_rbuf;
        retval = read(this->fd, this->rbuf + end_of_rbuf, remainingBytes);
        /// @note If read() has not read any data yet, it returns -1 and sets errno to EINTR
    } while (retval < 0 && errno == EINTR);
    // We loop over `read()` system call until
    // relval >= 0 (i.e reading wa successfull) or
    // when error has occured due to EAGAIN

    //
    // Handling retval
    //
    if (retval < 0 && errno == EAGAIN)
    {
        // If some process has the socket open for writing and
        // O_NONBLOCK is set to 1, read() returns -1 and
        // sets errno to EAGAIN.
        LOG("[INFO] Other process is using the current opened socket.");
        return false;
    }
    // else if not EAGAIN ... (other errors)
    if (retval < 0)
    {
        LOG("[ERROR] Unable to read from the socket due to `" + std::string(strerror(errno)) + " `");
        this->state = STATE_END;
        return false;
    }
    // else if zero bytes are been read ...
    if (retval == 0)
    {
        if (end_of_rbuf > 0)
        {
            LOG("[ERROR] Unexpected EOF.");
        }
        else
        {
            LOG("[INFO] Reached EOF");
        }
        this->state = STATE_END;
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
    while (this->oneRequest())
        ;
    /// @note in contrast to read_full we need to process the
    /// the received data as soon as we read it.
    // in order to free some space in the rbuff.

    return (this->state == STATE_REQ);
    // this function is intented to be called inside a loop
    // as long as it is possible to read new data
    // i.e the state of the connection should permit it
}

bool Connection::flushBuffer()
{
    ssize_t retval = 0;
    do
    {
        size_t remainingBytes = this->wbuf_size - this->wbuf_sent;
        retval = write(this->fd, this->wbuf + this->wbuf_sent, remainingBytes);
    } while (retval < 0 && errno == EINTR);

    //
    // Handling error
    //
    if (retval < 0 && errno == EAGAIN)
    {
        LOG("[INFO] Other process is using the current opened socket.");
        return false;
    }

    if (retval < 0)
    {
        LOG("[ERROR] Unable to write to the socket due to `" + std::string(strerror(errno)) + " `");
        this->state = STATE_END;
        return false;
    }

    //
    // ELSE retval >= 0
    //
    this->wbuf_sent += retval;
    assert(this->wbuf_sent <= this->wbuf_size);

    if (this->wbuf_sent == this->wbuf_size)
    {
        LOG("[INFO] No more data to write");
        this->state = STATE_REQ; // switch back to request state => the this-> is readable
        this->wbuf_sent = 0;
        this->wbuf_size = 0;
        return false;
    }

    // still more data to write
    return true;
}

bool Connection::parse_commands(uint8_t *request, size_t len) {

    std::vector<std::string> string_list;
    myRedis::str::decode_request((char*)request, string_list);

    // process the command
    for(size_t i = 0; i < string_list.size(); /* empty */) {
        std::string cmd = string_list[i];
        std::string key = string_list[i+1];
        std::string value;
        Connection::ret_code ret_code;
        std::string response_data;

        if (cmd == "set") {
            // parse additional args
            ///TODO: check if args exists
            value = string_list[i+2];
            ret_code = cmd_set(key, value);
            i += 3;

            response_data.clear();
            response_data.append("Set `")
            .append(key)
            .append("` to be `")
            .append(value)
            .append("`");

        } else if (cmd == "get") {
            // parse additional args
            ret_code = cmd_get(key, value);
            i += 2;

            response_data.clear();
            response_data = key + " = " + value;

        } else if (cmd == "del") {
            // parse additional args
            ret_code = cmd_del(key);
            i += 2;

            response_data.clear();
            response_data = "Deleted `" + key  + "`.";

        }else{
            LOG("[ERROR] Unknown command.");
            return true; // break the loop
        }

        // Reply to the request by inserting an response message
        myRedis::str::Response res;
        res.status_code = (myRedis::SIZE_T)ret_code;
        res.data_size = (myRedis::BYTE_T)response_data.size();
        memcpy(res.data, response_data.data(), res.data_size);

        myRedis::str::encode_response((char*)this->wbuf, this->wbuf_size, res);
    }

    return false;
}

Connection::ret_code Connection::cmd_set(const  std::string& key, const std::string& value) {
    if(m_server_data.find(key) == m_server_data.end()){
        // create
        m_server_data.insert(std::make_pair(key, value));
        return Connection::ret_code::CREATED;
    }else{
        // update
        m_server_data.at(key) = value;
        return Connection::ret_code::UPDATED;
    }
}

Connection::ret_code Connection::cmd_get(const std::string& key, std::string &ret_value) {
    if(m_server_data.find(key) == m_server_data.end()){
        // not found
        return Connection::ret_code::NOT_FOUND;
    }else{
        // found
        ret_value = m_server_data.at(key);
        return Connection::ret_code::FOUND;
    }
}

Connection::ret_code Connection::cmd_del(const std::string& key) {
    if(m_server_data.find(key) == m_server_data.end()){
        // create
        return Connection::ret_code::NOT_FOUND;
    }else{
        // delete
        m_server_data.erase(key);
        return Connection::ret_code::DELETED;
    }
}
