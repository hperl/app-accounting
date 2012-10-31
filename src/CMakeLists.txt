project(app-accounting C)
cmake_minimum_required(VERSION 2.8)
enable_testing()
include(Compiler/Clang-C)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ZeroMQ REQUIRED)
find_package(LibYAML REQUIRED)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_FLAGS                "-std=gnu99 -march=native -Qunused-arguments -Werror -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O4 -DNDEBUG -g")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
      message(FATAL_ERROR "In-source builds not allowed. Please make a new \
       directory (called a build directory) and run #CMake from there. You may \
       need to remove CMakeCache.txt.")
endif()

add_executable(
  wrapper
  wrapper.c
  symbols.h
)
set_target_properties(
  wrapper
  PROPERTIES
  INCLUDE_DIRECTORIES "${ZEROMQ_INCLUDE_DIRS}"  
)
target_link_libraries(
  wrapper
  ${ZEROMQ_LIBRARIES}
)

add_executable(
  server
  server.c
  xml.c
  server.h
  symbols.h
)
set_target_properties(
  server
  PROPERTIES
  INCLUDE_DIRECTORIES "${ZEROMQ_INCLUDE_DIRS}"  
)
target_link_libraries(
  server
  ${LIBYAML_LIBRARIES}
  ${ZEROMQ_LIBRARIES}
)
