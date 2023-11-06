cmake_minimum_required(VERSION 3.24.0)
set (CMAKE_CXX_STANDARD 20)

project(SPACEMATH CXX)
set( headers
    h/TimeScale.hpp
    h/Earth.hpp
    h/Sol.hpp
    )
set( sources 
    src/TimeScale.cpp
    src/Earth.cpp
    src/Sol.cpp
)

add_library( spacemath ${sources} ${headers})
target_include_directories( spacemath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/h )

add_library( spacemath_ut INTERFACE )
target_include_directories( spacemath_ut INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ut 
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/h 
                                                        )