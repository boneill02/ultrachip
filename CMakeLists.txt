# See https://honeytreelabs.com/posts/cmake-unity-integration

cmake_minimum_required(VERSION 4.0)
project("libc8" C)

set(TARGET_GROUP lib CACHE STRING "Group to build")

add_subdirectory(libc8)

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
