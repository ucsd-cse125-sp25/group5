cmake_minimum_required(VERSION 3.10)
project(MoonlightServer CXX)

set(CMAKE_C_STANDARD 11)

add_executable(server_app1
    main.cpp
    ServerGame.cpp
    ServerNetwork.cpp
    NetworkServices.cpp
    stdafx.cpp

  "include/physics/PhysicsData.h" "include/physics/PhysicsSystem.h" "physics/PhysicsSystem.cpp" "include/core.h" "include/physics/BehaviorComponent.h" "physics/PlayerBehaviorComponent.cpp" "physics/ProjectileBehaviorComponent.cpp" "physics/FlagBehaviorComponent.cpp"  "include/InputManager.h" "InputManager.cpp" "physics/MetalProjectileBehaviorComponent.cpp" "physics/PickupBehaviorComponent.cpp")


# Add include directories
include_directories(
    include
    "../include"
)

target_include_directories(server_app1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} "../include")

target_link_libraries(server_app1 PRIVATE ws2_32 mswsock advapi32)

target_compile_definitions(server_app1 PRIVATE _WIN32_WINNT=0x0601)

target_compile_features(server_app1 PRIVATE cxx_std_11)