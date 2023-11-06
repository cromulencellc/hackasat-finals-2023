cmake_minimum_required(VERSION 3.24.0)
set (CMAKE_CXX_STANDARD 20)

project(CFS_GEOFENCE CXX)


set( sources 
             src/Geofence.cpp
             src/GeofenceApp.cpp
             src/Zone.cpp
             h/Geofence.hpp
             h/GeofenceApp.hpp
             h/Zone.hpp
              )

# Create the app module
add_library( geofence_helpers ${sources})
target_include_directories( geofence_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/h  )
target_link_libraries( geofence_helpers PUBLIC spacemath cpp_base messages)

add_cfe_app( geofence entrypoint.cpp )
target_link_libraries( geofence PUBLIC messages cpp_base geofence_helpers spacemath )

target_include_directories( geofence PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/h)

add_library( geofence_ut INTERFACE )
target_include_directories( geofence_ut INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ut 
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/h 
                                                        )

