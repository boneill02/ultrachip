# See https://honeytreelabs.com/posts/cmake-unity-integration

cmake_minimum_required(VERSION 3.31.6)
project("libc8" C)

option(SDL2 "Use SDL2 in libc8" ON)
set(TARGET_GROUP lib CACHE STRING "Group to build")

# C standard
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_FILE_OFFSET_BITS=64")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-field-initializers")

add_subdirectory(libc8)
include_directories(libc8)

function(SDL2_Required)
  if(SDL2 EQUAL OFF)
    message(STATUS "SDL2 required for tools target group, setting SDL2 to ON")
    set(SDL2 ON)
  endif()
endfunction()

if(TARGET_GROUP STREQUAL test)
  include(CTest)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST")
  add_subdirectory(external)
  add_subdirectory(test)
elseif(TARGET_GROUP STREQUAL tools)
  SDL2_Required()
  add_subdirectory(tools)
elseif(TARGET_GROUP STREQUAL lib)
elseif(TARGET_GROUP STREQUAL all)
  SDL2_Required()
  add_subdirectory(external)
  add_subdirectory(tools)
elseif(TARGET_GROUP STREQUAL all-test)
  SDL2_Required()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST")
  add_subdirectory(external)
  add_subdirectory(tools)
  add_subdirectory(test)
else()
  message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()
