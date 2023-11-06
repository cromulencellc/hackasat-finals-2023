cmake_minimum_required(VERSION 3.24.0)

project(CFS_ATTITUDE CXX)


set( sources 
        src/entry.cpp
        src/Attitude.cpp
        src/Safe.cpp
              )
set( header 
        h/Attitude.hpp
        h/Safe.hpp
)


add_cfe_app( attitude  ${sources} ${headers})
target_link_libraries( attitude PUBLIC  messages cpp_base spacemath)
target_include_directories( attitude PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/h)


add_library( attitude_ut INTERFACE )
target_include_directories( attitude_ut INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ut 
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/h 
                                                        )

