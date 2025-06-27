set(INTERPRETER_BINARY_NAME "chip8")
set(ASSEMBLER_BINARY_NAME "chip8as")
set(DISASSEMBLER_BINARY_NAME "chip8dis")

# Get git commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_executable(${INTERPRETER_BINARY_NAME} chip8.c)
add_executable(${ASSEMBLER_BINARY_NAME} chip8as.c)
add_executable(${DISASSEMBLER_BINARY_NAME} chip8dis.c)

# Set the version for the executables
target_compile_definitions(${INTERPRETER_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")
target_compile_definitions(${ASSEMBLER_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")
target_compile_definitions(${DISASSEMBLER_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")

target_link_libraries(${INTERPRETER_BINARY_NAME} PRIVATE c8)
target_link_libraries(${ASSEMBLER_BINARY_NAME} PRIVATE c8)
target_link_libraries(${DISASSEMBLER_BINARY_NAME} PRIVATE c8)

# Link -lSDL2 for chip8 only
target_link_libraries(${INTERPRETER_BINARY_NAME} PRIVATE SDL2)
