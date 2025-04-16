cmake_minimum_required(VERSION 3.10)
project(MoonlightServer CXX)

set(CMAKE_C_STANDARD 11)

add_executable(server_app
    main.cpp
    ServerGame.cpp
    ServerNetwork.cpp
    NetworkServices.cpp
    stdafx.cpp
 )

# Add include directories
include_directories(
    include
)

target_include_directories(server_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(server_app PRIVATE ws2_32 mswsock advapi32)

target_compile_definitions(server_app PRIVATE _WIN32_WINNT=0x0601)