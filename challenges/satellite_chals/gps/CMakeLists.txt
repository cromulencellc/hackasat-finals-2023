cmake_minimum_required(VERSION 3.24.0)

project(CFS_GPS CXX)


set( sources 
        src/Gps.cpp
        src/NovatelSerializer.cpp
        entrypoint.cpp
              )
set( header 
        h/BestPos.hpp
        h/Gps.hpp
        h/LogParser.hpp
        h/NovatelSerializer.hpp
)


add_cfe_app( gps entrypoint.cpp  ${sources} ${headers})
target_link_libraries( gps PUBLIC messages cpp_base spacemath )
target_include_directories( gps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/h
                                       )


add_library( gps_ut INTERFACE )
target_include_directories( gps_ut INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ut 
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/h
                                                        )

