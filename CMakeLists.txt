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

add_library(
  acc_preload
  SHARED
  symbols.h
  acc_preload.c
)
set_target_properties(
  acc_preload
  PROPERTIES
  INCLUDE_DIRECTORIES "${ZEROMQ_INCLUDE_DIRS}"  
)
target_link_libraries(
  acc_preload
  ${ZEROMQ_LIBRARIES}
)

add_executable(
  acc_server
  server.c
  xml.c
  server.h
  symbols.h
)
set_target_properties(
  acc_server
  PROPERTIES
  INCLUDE_DIRECTORIES "${ZEROMQ_INCLUDE_DIRS}"  
)
target_link_libraries(
  acc_server
  ${LIBYAML_LIBRARIES}
  ${ZEROMQ_LIBRARIES}
)

install(
  TARGETS acc_preload acc_server
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
)
install(
  PROGRAMS acc_bash
  DESTINATION bin
)

set(CPACK_GENERATOR "RPM" "DEB")
set(CPACK_PACKAGE_NAME "app-accounting")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This software supplies a sensor for application accounting, with the goal to        account start and end time as well as the name of the launched programs.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_CONTACT "Henning Perl <perl@dcsec.uni-hannover.de>")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "2")
set(CPACK_PACKAGE_VENDOR "Leibniz University Hannover")

# DEB settings
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.11.3), libyaml-0-2 (>= 0.1.4), libzmq1 (>= 2.2.0)")
set(CPACK_DEBIAN_PACKAGE_SECTION "admin")
# RPM settings
set(CPACK_RPM_PACKAGE_REQUIRES "libc6 >= 2.11.3, libyaml-0-2 >= 0.1.4, libzmq1 >= 2.2.0")

include(CPack)

