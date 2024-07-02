project(multiplexing)

set(CMAKE_CXX_STANDARD 17)

include_directories(epoll poll select common)

add_executable(selectServer select/selectServer.cpp
        )

add_executable(pollServer poll/pollServer.cpp)

add_executable(EpollServer epoll/v1/EpollServer.cpp)

add_executable(EpollServerV2 epoll/v2/epollServerV2.cpp)

#target_link_libraries(untitled5 )
#target_link_libraries(pollserver )