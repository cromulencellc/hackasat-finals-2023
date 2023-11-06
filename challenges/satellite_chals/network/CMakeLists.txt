project(Network CXX)

set( headers
    h/UdpClient.hpp
    h/UdpServer.hpp
    )
set( sources 
    src/UdpClient.cpp
    src/UdpServer.cpp
)

add_library( network ) 
target_sources( network PRIVATE ${sources} ${headers} )
target_link_libraries( network PUBLIC cpp_base )
target_include_directories( network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/h )
