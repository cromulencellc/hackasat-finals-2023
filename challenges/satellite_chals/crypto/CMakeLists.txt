
cmake_minimum_required(VERSION 3.24.0)
project(CRYPTO CXX)


set( sources
    h/crypto.hpp
)
find_package(OpenSSL REQUIRED)

# Create the app module
add_library( crypto INTERFACE ${sources} )
target_include_directories( crypto INTERFACE 
                            ${CMAKE_CURRENT_SOURCE_DIR}/h )


