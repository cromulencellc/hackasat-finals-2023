cmake_minimum_required( VERSION 3.5 )

# Set project name
project( pwdmanager )

set(SRC_FILES
    main.cpp
)

add_subdirectory( manager )
add_executable( finalpass main.cpp )

target_link_libraries( finalpass PUBLIC manager )
target_compile_features( finalpass PRIVATE cxx_std_17 )