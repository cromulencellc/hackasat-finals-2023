cmake_minimum_required(VERSION 3.24.0)
project(CFS_CPPBASE CXX)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)


set( sources 
    src/AppBase.cpp
    src/ASyncApp.cpp
    src/PacketFilter.cpp
    src/RateApp.cpp
    src/secrets.cpp
    src/TimerApp.cpp
    src/subprocess.cpp
    entrypoint.cpp
)

set( headers 
    h/AppBase.hpp
    h/ASyncApp.hpp
    h/BitFields.hpp
    h/ClockFrameApp.hpp
    h/Message.h
    h/PacketFilter.hpp
    h/Packetizer.hpp
    h/TimerApp.hpp
    h/subprocess.hpp
)

# Create the app module
add_library( cpp_base INTERFACE  )
add_cfe_app( crom_cpp ${sources} ${headers})

target_include_directories( cpp_base INTERFACE 
                           ${cfs_lib_MISSION_DIR}/fsw/public_inc
                           ${CMAKE_CURRENT_SOURCE_DIR}/h
                         )

message( STATUS "Linkzor")
target_link_libraries( cpp_base INTERFACE nlohmann_json::nlohmann_json containers)
target_link_libraries( crom_cpp PUBLIC cpp_base)

target_compile_options( cpp_base INTERFACE -std=c++2a -s)
target_compile_options( crom_cpp PUBLIC -std=c++2a -s )

add_library( cpp_ut INTERFACE )
target_include_directories( cpp_ut INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ut )
target_link_libraries( cpp_ut INTERFACE containers messages)
