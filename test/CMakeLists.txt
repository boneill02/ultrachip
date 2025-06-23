add_executable(test_chip8
	test_chip8.c
)

target_link_libraries(test_chip8
	libc8
	Unity
)

add_test(chip8 test_chip8)
