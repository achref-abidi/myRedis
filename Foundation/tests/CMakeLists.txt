
cmake_minimum_required(VERSION 3.20)

add_executable(Test_myRedis main.cpp)
target_link_libraries(Test_myRedis ${PROJECT_NAME})