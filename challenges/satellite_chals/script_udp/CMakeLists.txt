cmake_minimum_required(VERSION 3.24.0)

project(CFS_SCRIPTUDP CXX)


set( sources 
        src/entry.cpp
        src/ScriptUdp.cpp
              )
set( header 
        h/ScriptUdp.hpp
)


add_cfe_app( script_udp  ${sources} ${headers})
target_link_libraries( script_udp PUBLIC network messages cpp_base )
target_include_directories( script_udp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/h)



add_library( scriptudp_ut INTERFACE )
target_include_directories( scriptudp_ut INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ut 
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/h 
                                                        )

