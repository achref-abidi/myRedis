//
// Created by achref abidi on 22/07/23.
//
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <functional>


namespace myRedis{
    using BYTE_T = uint8_t;
    using SIZE_T = uint32_t; // the size of a field that contains size information
    const SIZE_T k_max_msg = 4096; ///< Equals to 4 KBytes of data.


    struct Command{
        std::string name;
        uint8_t nbr_args;
        std::function<SIZE_T(std::initializer_list<std::string>)> command_action;
    };

    // insert a response with its len
    // decode a request i.e.

    namespace str{

        struct Response{
            SIZE_T status_code;
            BYTE_T data[k_max_msg];
            SIZE_T data_size;
        };

        //
        // @brief A helper function to encode a response according
        // to a specific protocol and add it to `dest` buffer.
        // @note A response have 3 fields : res_len | status_code | data
        // + response buffer length ( 4 bytes = sizeof(SIZE_T))
        // + status code ( 4 bytes = sizeof(SIZE_T))
        // + actual data
        // @param dest Destination buffer where the encoded response
        // will be added.
        // @param dest_size Used to update the size of the `dest` buffer.
        // @param res
        //todo: should send also the length of the packet??? or the client should handle that knowing how many cmd  sent
        static void encode_response(char* dest, SIZE_T& dest_size, const Response& res){
            // Denotes the location at which to insert data
            dest += dest_size;

            // Inserting header which is the total size of data + status code
            SIZE_T header = res.data_size + sizeof(SIZE_T);
            memcpy(dest, &header, sizeof(SIZE_T));
            dest += sizeof(SIZE_T);

            // Inserting status code
            memcpy(dest, &res.status_code, sizeof(SIZE_T));
            dest += sizeof(SIZE_T);

            // Inserting data
            memcpy(dest, res.data, res.data_size);

            // Updating destination size
            dest_size += sizeof(SIZE_T) * 2 + res.data_size;
        }

        static void decode_response(char* src, Response& res){
            SIZE_T header;
            memcpy(&header, src, sizeof(SIZE_T));
            src += sizeof(SIZE_T);
            res.data_size = header - sizeof(SIZE_T);//todo check the size of header > SIZE_T

            memcpy(&res.status_code, src, sizeof(SIZE_T));
            src += sizeof(SIZE_T);

            memcpy(res.data, src, res.data_size);
        }

        /**
         *
         * @param dest
         * @param len_ Length of the final encoded request.
         * @param string_list
         */
        static void encode_request(char* dest , SIZE_T & len_, std::vector<std::string> string_list){
            SIZE_T nbr_string = string_list.size();
            memcpy(dest, &nbr_string, sizeof(SIZE_T));
            dest += sizeof(SIZE_T);

            // for each string insert its len
            len_ = 0;
            for(size_t i = 0; i< nbr_string; i++){
                // insert the string len
                SIZE_T len = string_list.at(i).size();
                memcpy(dest, &len, sizeof(SIZE_T));
                dest += sizeof(SIZE_T);

                // insert the string
                char* string = string_list.at(i).data();
                memcpy(dest, string, len);
                dest += len;

                len_ += len;
            }

            len_ += sizeof(SIZE_T) * (nbr_string + 1);
        }

        static void decode_request(char* src, std::vector<std::string>& string_list){
            ///TODO: use string view
            // get the number of strings
            SIZE_T nbr_string;
            memcpy(&nbr_string, src, sizeof(SIZE_T));
            src += sizeof(SIZE_T); // the byte from which starts reading individual strings

            // assume nbr_string > 0

            // each string has a len
            // each string is get key , set key value, del key
            // nbrstr | lenstr1 | str1 | lenstr2 | ...

            string_list.clear();
            string_list.reserve(nbr_string);

            for(size_t i = 0; i< nbr_string; i++){
                /// TODO:check if data is actually available

                // parse the string len
                SIZE_T len;
                memcpy(&len, src, sizeof(SIZE_T));
                src += sizeof(SIZE_T);

                // parse string
                unsigned char string [k_max_msg];
                memcpy(string, src, len);
                src += len;

                string[len] = '\0';
                string_list.emplace_back((const char*)string);
            }
        }
    }
}
//todo: read for client takes time
//todo: add sig to this file
//todo: add Core.cpp
//todo: add namespaces to server and ather files
// todo: reformat comments
//todo: rename client to application