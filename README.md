# libc8

This is a C library for interpreting, assembling, and disassembling CHIP-8 and
SCHIP code, optionally utilizing the SDL2 library for graphics.

An example assembler, disassembler, and interpreter is located in `tools/`.

## Building

```
cmake .
```

This will build libc8 as well as the example tools.

### SDL2

SDL2 support is enabled by default. To disable it to use another graphics
library, run `cmake` with `-DSDL2=OFF`. If `simulate()` is ever called in your
code, you must implement these functions with your preferred graphics library:

* `void c8_deinit_graphics(void)`
* `int c8_init_graphics(void)`
* `void c8_render(c8_display_t *, int *)`
* `int c8_tick(int *, int)`

See [this page](https://oneill.sh/doc/libc8/graphics__sdl2_8c.html#a04f712dc6e338364ae5e43e0b6ae9762)
for more information about these functions.

## Example Tools

* See [here](doc/c8.md) for information about the example libc8 interpreter,
  `c8`.
* See [here](doc/c8as.md) for information about the example libc8 assembler,
  `c8as`.
* See [here](doc/c8dis.md) for information about the example libc8 disassembler,
  `c8dis`.

## Documentation

Full documentation for libc8 is available [here](https://oneill.sh/docs/libc8).

## Testing

Testing is done using
[ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) and
[Unity](https://github.com/ThrowTheSwitch/Unity) (See
[here](https://honeytreelabs.com/posts/cmake-unity-integration) for an
overview).

```
cmake -DTARGET_GROUP=test
make && ctest --verbose
```

## Further Reading

* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [S-CHIP 1.1 Reference](http://devernay.free.fr/hacks/chip8/schip.txt)
* [Octo](https://github.com/JohnEarnest/Octo)
* [CHIP-8 Extensions and Compatibility](https://chip-8.github.io/extensions/)

## License

Copyright (c) 2019-2025 Ben O'Neill <ben@oneill.sh>. Licensed under the
MIT License. See LICENSE.
