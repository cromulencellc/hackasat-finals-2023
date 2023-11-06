# Find required Qt packages
# find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenSSL REQUIRED)

# Set source files
set(SRC_FILES
    manager.cpp
    user.cpp
    crypto.cpp
    helper.cpp
)

set(HEADER_FILES 
    manager.h
    user.h
    crypto.h
    helper.h
)

# Add executable
add_library( manager ${SRC_FILES} ${HEADER_FILES} )

# Link against Qt libraries
target_include_directories( manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( manager OpenSSL::Crypto) # Qt5::Widgets)
target_compile_features( manager PRIVATE cxx_std_17) 