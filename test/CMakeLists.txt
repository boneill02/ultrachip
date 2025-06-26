add_executable(chip8_tests
	test_chip8.c
)
target_link_libraries(chip8_tests
	c8
	Unity
)
add_test(chip8 chip8_tests)

add_executable(debug_tests
	test_debug.c
)
target_link_libraries(debug_tests
	c8
	Unity
)
add_test(debug debug_tests)

add_executable(decode_tests
	test_decode.c
)
target_link_libraries(decode_tests
	c8
	Unity
)
add_test(decode decode_tests)

add_executable(encode_tests
	test_encode.c
)
target_link_libraries(encode_tests
	c8
	Unity
)
add_test(encode encode_tests)

add_executable(exception_tests
	test_exception.c
)
target_link_libraries(exception_tests
	c8
	Unity
)
add_test(exception exception_tests)

add_executable(graphics_tests
	test_graphics.c
)
target_link_libraries(graphics_tests
	c8
	Unity
)
add_test(graphics graphics_tests)

add_executable(symbol_tests
	test_symbol.c
)
target_link_libraries(symbol_tests
	c8
	Unity
)
add_test(symbol symbol_tests)

add_executable(util_tests
	test_util.c
)
target_link_libraries(util_tests
	c8
	Unity
)
add_test(util util_tests)
