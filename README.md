# libc8

[![Build Status](https://github.com/boneill02/libc8/actions/workflows/cmake-single-platform.yml/badge.svg?branch=master)](https://github.com/boneill02/libc8/actions/workflows/cmake-single-platform.yml).

This is a C library for interpreting, assembling, and disassembling CHIP-8 and
SCHIP code, optionally utilizing the [SDL2](https://www.libsdl.org/) library
for graphics.

An example [assembler](doc/chip8as.md), [disassembler](doc/chip8dis.md), and
[interpreter](doc/chip8.md) is located in `tools/`.

## Building

```shell
cmake -DTARGET_GROUP=all .
make
```

This will build libc8 as well as the example tools and tests.

### SDL2

SDL2 support is enabled by default. To disable it to use another graphics
library, run `cmake` with `-DSDL2=OFF`. If `simulate()` is ever called in your
code, you must implement these functions with your preferred graphics library:

**Note**: the `all` and `tools` targets require `SDL2` to be `ON`.

* `void c8_deinit_graphics(void)`
* `int c8_init_graphics(void)`
* `void c8_render(c8_display_t *, int *)`
* `int c8_tick(int *, int)`

See [this page](https://oneill.sh/doc/libc8/graphics__sdl2_8c.html#a04f712dc6e338364ae5e43e0b6ae9762)
for more information about these functions.

## Documentation

[Full documentation for libc8](https://boneill02.github.io/libc8/doc/html) is
available on the GitHub pages site.

## Testing

Testing is done using
[ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) and
[Unity](https://github.com/ThrowTheSwitch/Unity) (See
[here](https://honeytreelabs.com/posts/cmake-unity-integration) for an
overview).

```
cmake -DTARGET_GROUP=test -DSDL2=OFF
make && ctest --verbose
```

**Note**: When built with tests enabled (`test` and `all-test` `TARGET_GROUP`s),
`libc8` will not halt execution after encountering an error, potentially leading
to undefined behavior.

## Showcase

The libc8 CHIP-8 interpreter running [Outlaw by John Earnest](https://johnearnest.github.io/chip8Archive/play.html?p=outlaw):

![libc8 running outlaw](https://oneill.sh/img/libc8-outlaw.gif)

## Further Reading

* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [S-CHIP 1.1 Reference](http://devernay.free.fr/hacks/chip8/schip.txt)
* [Octo](https://github.com/JohnEarnest/Octo)
* [CHIP-8 Extensions and Compatibility](https://chip-8.github.io/extensions/)

## Bugs

If you find a bug, submit an issue, PR, or email me with a description and/or patch.

## License

Copyright (c) 2019-2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
