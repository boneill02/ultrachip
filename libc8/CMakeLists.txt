
project("libc8" C)

# Global Parameters

set(PROJECT_NAME "libc8")

set(LIBRARY_NAME "c8")
set(LIBRARY_VERSION_MAJOR 0)
set(LIBRARY_VERSION_STRING 0.0)

include(GNUInstallDirs)

set(LIBRARY_BASE_PATH ".")

INCLUDE_DIRECTORIES (
	"${LIBRARY_BASE_PATH}"
)

set(LIBRARY_PUBLIC_SRC
	"${LIBRARY_BASE_PATH}/c8/chip8.c"
	"${LIBRARY_BASE_PATH}/c8/decode.c"
	"${LIBRARY_BASE_PATH}/c8/encode.c"
	"${LIBRARY_BASE_PATH}/c8/font.c"
	"${LIBRARY_BASE_PATH}/c8/graphics.c"
)

set(LIBRARY_PRIVATE_SRC
	"${LIBRARY_BASE_PATH}/c8/private/debug.c"
	"${LIBRARY_BASE_PATH}/c8/private/exception.c"
	"${LIBRARY_BASE_PATH}/c8/private/instruction.c"
	"${LIBRARY_BASE_PATH}/c8/private/symbol.c"
	"${LIBRARY_BASE_PATH}/c8/private/util.c"
)

set(LIBRARY_PUBLIC_HEADERS
	"${LIBRARY_BASE_PATH}/chip8.h"
	"${LIBRARY_BASE_PATH}/decode.h"
	"${LIBRARY_BASE_PATH}/encode.h"
	"${LIBRARY_BASE_PATH}/font.h"
	"${LIBRARY_BASE_PATH}/graphics.h"
)

set(LIBRARY_PRIVATE_HEADERS
	"${LIBRARY_BASE_PATH}/c8/private/debug.h"
	"${LIBRARY_BASE_PATH}/c8/private/exception.h"
	"${LIBRARY_BASE_PATH}/c8/private/instruction.h"
	"${LIBRARY_BASE_PATH}/c8/private/symbol.h"
	"${LIBRARY_BASE_PATH}/c8/private/util.h"
)

if(SDL2)
    list(APPEND LIBRARY_PRIVATE_SRC "${LIBRARY_BASE_PATH}/c8/private/graphics_sdl2.c")
endif()

add_library (
	${LIBRARY_NAME} SHARED ${LIBRARY_PUBLIC_SRC} ${LIBRARY_PRIVATE_SRC}
)

if(SDL2)
    target_link_libraries(${LIBRARY_NAME} PRIVATE SDL2)
endif()

set_target_properties(
	${BINARY_NAME} PROPERTIES
	VERSION		${LIBRARY_VERSION_STRING}
	SOVERSION	${LIBRARY_VERSION_MAJOR}
	PUBLIC_HEADER  ${LIBRARY_PUBLIC_HEADERS}
)

# Compiler definitions
ADD_DEFINITIONS("-g")
if(TEST)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST")
endif()

# Library installation directives
INSTALL (
	TARGETS ${BINARY_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)