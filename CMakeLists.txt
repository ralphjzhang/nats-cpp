cmake_minimum_required(VERSION 3.29)

# set(CMAKE_C_COMPILER /usr/bin/gcc)
# set(CMAKE_CXX_COMPILER /usr/bin/g++)

project(nats-cpp
  LANGUAGES CXX
  VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_LINKER_TYPE LLD)
set(CMAKE_CXX_FLAGS "-Wpedantic -Wall -Wextra")

# find_package(OpenSSL REQUIRED)

include_directories(
  "/usr/include"
  "${CMAKE_SOURCE_DIR}/inc/asio"
  "${CMAKE_SOURCE_DIR}/inc"
)

link_libraries(pthread)

add_executable(st
  st/main.cpp
)

add_executable(ut
  ut/main.cpp
)

add_test(NAME ut
  COMMAND ${CMAKE_BINARY_DIR}/ut
)
