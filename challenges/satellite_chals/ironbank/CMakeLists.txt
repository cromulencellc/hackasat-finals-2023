cmake_minimum_required(VERSION 3.24.0)
set (CMAKE_CXX_STANDARD 20)
project( IronBank CXX )


set( sources src/IronBank.cpp
             
    )

set( headers h/IronBank.hpp
             h/i686/clocks.hpp
             h/riscv/clocks.hpp
             shared/ib_messages.hpp
             )
find_package(OpenSSL REQUIRED)

add_library( ironbank ${sources} ${headers} )
target_compile_options( ironbank PRIVATE -O0 )

target_link_libraries( ironbank PUBLIC crypto network OpenSSL::Crypto  nlohmann_json::nlohmann_json )

target_include_directories( ironbank PUBLIC 
                                      ${CMAKE_CURRENT_SOURCE_DIR}/h
                                      $<$<STREQUAL:i686,${CMAKE_SYSTEM_PROCESSOR}>:${CMAKE_CURRENT_SOURCE_DIR}/h/i686>
                                      $<$<STREQUAL:rv32,${CMAKE_SYSTEM_PROCESSOR}>:${CMAKE_CURRENT_SOURCE_DIR}/h/riscv>
                                     PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/shared )
##################
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
####################

add_executable( ironbank_ut ut/ut_main.cpp  )
target_include_directories( ironbank_ut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ut )
target_link_libraries(ironbank_ut PUBLIC ironbank gtest crypto network OpenSSL::Crypto  nlohmann_json::nlohmann_json  )

add_executable( ironbankapp src/main.cpp )
target_link_libraries( ironbankapp PRIVATE ironbank )
target_compile_options( ironbankapp PRIVATE -O0 )


#install( TARGETS ironbank_ut 
#         DESTINATION DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGETSYSTEM}
#)


install( TARGETS ironbankapp
         DESTINATION DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGETSYSTEM}/cf
)