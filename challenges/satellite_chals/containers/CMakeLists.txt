cmake_minimum_required(VERSION 3.24.0)
project(CPP_CONTAINERS CXX)


set( sources
    h/circular.hpp
    h/persistent.hpp
    h/key_queue.hpp
)
# Create the app module
add_library( containers INTERFACE ${sources} )
target_include_directories( containers INTERFACE 
                                 ${CMAKE_CURRENT_SOURCE_DIR}/h
                                  )
add_library( container_ut INTERFACE )
target_include_directories( container_ut INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ut )
target_link_libraries(container_ut INTERFACE containers)