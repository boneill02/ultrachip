# See https://honeytreelabs.com/posts/cmake-unity-integration

cmake_minimum_required(VERSION 3.31.6)
project("libc8" C)

option(SDL2 "Use SDL2 in libc8" ON)
set(TARGET_GROUP lib CACHE STRING "Group to build")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-field-initializers")

add_subdirectory(libc8)
include_directories(libc8)

if(TARGET_GROUP STREQUAL test)
  include(CTest)
  add_subdirectory(external)
  add_subdirectory(test)
elseif(TARGET_GROUP STREQUAL tools)

  if(SDL2 EQUAL OFF)
    message(STATUS "SDL2 required for tools target group, setting SDL2 to ON")
    set(SDL2 ON)
  endif()

  add_subdirectory(tools)
elseif(TARGET_GROUP STREQUAL lib)
elseif(TARGET_GROUP STREQUAL all)

  if(SDL2 EQUAL OFF)
    message(STATUS "SDL2 required for all target group, setting SDL2 to ON")
    set(SDL2 ON)
  endif()

  add_subdirectory(external)
  add_subdirectory(test)
  add_subdirectory(tools)
else()
  message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()
