//
// @file Client.cpp
// @headerfile Client.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 21 May 2023
// @brief
//
#include "myRedis/Client.h"

bool Application::initConnection() {
    //
    // Socket
    //
    m_fd = socket(AF_INET, SOCK_STREAM, (int)0);
    if (m_fd == -1)
    {
        std::cout << "error\n";
        return false;
    }

    //
    // connect
    //
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    int rv = connect(m_fd, (const sockaddr *)&addr, sizeof((addr)));
    if (rv)
    {
        std::cout << "errro\n";
        return false;
    }

    return true;
}

bool Application::closeConnection() {
    close(m_fd);
    return true;
}

bool Application::get(const std::string &key, myRedis::str::Response& res) {
    return query({"get", key}, res);
}

bool Application::query(const std::vector<std::string> &q, myRedis::str::Response& res, int nbr_cmd) {
    myRedis::SIZE_T len;

    // Constructing the message frame according to our protocol
    char wbuf[4 + myRedis::k_max_msg];

    myRedis::str::encode_request(wbuf, len, q);

    // sending the message
    if (int32_t err = write_full(m_fd, wbuf, len)) ///TODO: use Connection flushBuffer function
    {
        return err;
    }

    while(nbr_cmd > 0) {
        // Reading server response len
        char rbuf[4 + myRedis::k_max_msg + 1];
        errno = 0;
        int32_t err = read_full(m_fd, rbuf, sizeof(myRedis::SIZE_T));
        if (err) {
            if (errno == 0) {
                std::cout << "EOF\n";
            } else {
                std::cout << "Error while reading\n";
            }
            return err;
        }

        myRedis::SIZE_T res_len; //i.e header
        memcpy(&res_len, rbuf, sizeof(myRedis::SIZE_T));
        err = read_full(m_fd, rbuf + sizeof(myRedis::SIZE_T), res_len);
        if (err) {
            if (errno == 0) {
                std::cout << "EOF\n";
            } else {
                std::cout << "Error while reading\n";
            }
            return err;
        }

        //myRedis::str::Response res {};
        res = {};
        myRedis::str::decode_response(rbuf, res);

        nbr_cmd--;
    }
    return 0;
}

bool Application::set(const std::string &key, const std::string &value, myRedis::str::Response& res) {
    return query({"set", key, value}, res);
}

bool Application::del(const std::string &key, myRedis::str::Response& res) {
    return query({"del", key}, res);
}
