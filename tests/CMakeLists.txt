
cmake_minimum_required(VERSION 3.20)

set(BUILD_CLINET_AND_SERVER ON CACHE INTERNAL "Building a server and a client app to play with")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/)

#add_executable(Test_myRedis main.cpp)
#target_link_libraries(Test_myRedis ${PROJECT_NAME})

if(BUILD_CLINET_AND_SERVER)
    add_executable(Test_client "./client.cpp")
    target_link_libraries(Test_client ${PROJECT_NAME})

    add_executable(Test_server "./server.cpp")
    target_link_libraries(Test_server ${PROJECT_NAME})
endif()