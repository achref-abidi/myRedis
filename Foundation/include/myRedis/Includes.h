//
// @file Includes.h
// @author Achref Abidi (https://github.com/achref-abidi)
// @date 29 June 2023
// @brief Holds a simple logging function, and a constant
// for the max numbers of bytes that the server or client
// can deal with it.
//
#pragma once

#include <iostream>

const size_t k_max_msg = 4096; ///< Equals to 4 KBytes of data.

#define DEBUG

#ifdef DEBUG
#ifdef __GNUG__
#define LOG(msg)                               \
    (std::cout << __FILE_NAME__                \
               << "(" << __LINE__ << ") : "    \
               << __PRETTY_FUNCTION__ << " : " \
               << msg                          \
               << std::endl)

#endif // __GNUG__
#else
#define LOG(msg)
#endif // DEBUG