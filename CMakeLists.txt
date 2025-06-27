# See https://honeytreelabs.com/posts/cmake-unity-integration

cmake_minimum_required(VERSION 3.31.6)
project("libc8" C)

option(SDL2 "Use SDL2 in libc8" ON)
set(TARGET_GROUP lib CACHE STRING "Group to build")

add_subdirectory(libc8)
include_directories(libc8)

if(TARGET_GROUP STREQUAL test)
  include(CTest)
  add_subdirectory(external)
  add_subdirectory(test)
elseif(TARGET_GROUP STREQUAL tools)
  add_subdirectory(tools)
elseif(TARGET_GROUP STREQUAL lib)
else()
  message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()
