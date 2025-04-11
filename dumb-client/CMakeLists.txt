cmake_minimum_required(VERSION 3.10)
project(MoonlightClient CXX)

set(CMAKE_C_STANDARD 11)

add_executable(client_app
    main.cpp
    ClientGame.cpp
    ClientNetwork.cpp
    NetworkServices.cpp
    stdafx.cpp
)

target_include_directories(client_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(client_app PRIVATE ws2_32 mswsock advapi32)

target_compile_definitions(client_app PRIVATE _WIN32_WINNT=0x0601)
