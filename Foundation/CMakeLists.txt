
#set(MY_REDIS_INC
#    ./include/myRedis/Server.h
#    ./include/myRedis/Connection.h
#)

set(MY_REDIS_SRC
    ./src/Server.cpp
    ./src/Connection.cpp
    ./src/AsyncServer.cpp
)

add_library(${PROJECT_NAME} STATIC ${MY_REDIS_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC "./include/")