# See https://honeytreelabs.com/posts/cmake-unity-integration

cmake_minimum_required(VERSION 3.31.6)
project("libc8" C)
include(CTest)

# defaults
set(SDL2 OFF)

# C standard
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_FILE_OFFSET_BITS=64")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-field-initializers")

function(SDL2_Required)
  if(SDL2 EQUAL OFF)
    message(STATUS "SDL2 required for tools target group, setting SDL2 to ON")
    set(SDL2 ON CACHE INTERNAL "")
  endif()
endfunction()

function(Enable_Tests)
  SDL2_Required()
  set(TEST ON CACHE INTERNAL "")
endfunction()

function(Build_Library)
  add_subdirectory(libc8)
  include_directories(libc8)
endfunction()

function(Build_Tests)
  add_subdirectory(external)
  add_subdirectory(test)
endfunction()

if(TARGET_GROUP STREQUAL test)
  Enable_Tests()
  Build_Library()
  Build_Tests()
elseif(TARGET_GROUP STREQUAL tools)
  SDL2_Required()
  Build_Library()
  add_subdirectory(tools)
elseif(TARGET_GROUP STREQUAL lib)
  Build_Library()
elseif(TARGET_GROUP STREQUAL all)
  SDL2_Required()
  Build_Library()
  add_subdirectory(tools)
elseif(TARGET_GROUP STREQUAL all-test)
  Enable_Tests()
  Build_Library()
  Build_Tests()
  add_subdirectory(tools)
else()
  message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()

