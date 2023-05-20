

set(MY_REDIS_SRC
    
)

set(MY_REDIS_INC
    
)

add_subdirectory(tests)

add_library(${PROJECT_NAME} STATIC ${MY_REDIS_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC "./include/")