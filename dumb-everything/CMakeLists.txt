cmake_minimum_required(VERSION 3.10)
project(MoonlightMatsuriServerClient CXX)

# Use C11
set(CMAKE_C_STANDARD 11)

# Your source files
set(SOURCES
    main.cpp
    stdafx.cpp
    ClientGame.cpp
    ClientNetwork.cpp
    ServerGame.cpp
    ServerNetwork.cpp
    NetworkServices.cpp
)

# Your headers (optional to list, but nice for IDEs)
set(HEADERS
    stdafx.h
    targetver.h
    ClientGame.h
    ClientNetwork.h
    ServerGame.h
    ServerNetwork.h
    NetworkData.h
    NetworkServices.h
)

add_executable(moonlight_matsuri ${SOURCES} ${HEADERS})

# Link with Windows networking libs
target_link_libraries(moonlight_matsuri
    ws2_32
    mswsock
    advapi32
)

# Let Windows define Win32 version macros
target_compile_definitions(moonlight_matsuri PRIVATE _WIN32_WINNT=0x0601)
