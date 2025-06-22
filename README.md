# libc8

This is a C library for interpreting, assembling, and disassembling CHIP-8 and
SCHIP code, optionally utilizing the SDL2 library for graphics.

An example assembler, disassembler, and interpreter is located in `tools/`.

## Building

`make libc8`

### SDL2

SDL2 support is disabled by default. To enable, uncomment the SDL2-related flags
in `config.mk` and set `SDL2_PATH` to the directory containing your `libsdl2.so`
and `libsdl2.la` (if building libc8 statically) files.

### Other graphics libraries

Any C graphics library can be used with libc8. To use a graphics library other
than SDL2, make sure the SDL2 flags in `config.mk` are commented out before
building libc8, and implement the following functions in your code:

* `void c8_deinit_graphics(void)`
* `int c8_init_graphics(void)`
* `void c8_render(c8_display_t *, int *)`
* `int c8_tick(int *, int)`

## CHIP-8 Interpreter

This is a fully functional CHIP-8 and SCHIP interpreter with an
integrated debug mode, utilizing libc8 with SDL2.

### Usage

```
c8 [-dvV] [-c clockspeed] [-f small,big] [-p file] [-P colors] [-q quirks] file
```

* `-c` sets the number of instructions to be executed per second (default: 500).
* `-d` enables debug mode. This can be used to add breakpoints, display the
  current memory, and step through instructions individually.
* `-f` loads the specified comma-separated fonts. Big font is optional.
* `-p` loads a color palette from a file containing two newline-separated 24-bit hex codes.
* `-P` sets the color palette from a string containing two comma-separated 24-bit hex codes.
* `-q` sets the quirks to enable from string with non-separated quirk identifiers
* `-v` enables verbose mode. This will print each instruction that is executed.
* `-V` prints the version number.

Keyboard layout is the following:

```
1 2 3 4
q w e r
a s d f
z x c v
```

### Fonts

Same as Octo.

Small fonts:

* octo
* vip
* dream6800
* eti660
* fish

Big fonts:

* octo
* schip
* fish

### Quirks

The following quirks are available:

* `b`: After `AND`, `OR`, and `XOR`: Clear `VF`.
* `d`: For `DRW`: Clip sprites at edge of display.
* `j`: For `JP V0, nnn`: Jump to `nnn + V[(nnn>>8)&0xF]` instead.
* `l`: After `LD [I], Vx` and `LD Vx, [I]`: Set `I` to address
  `I + x + 1`.
* `s`: Before `SHL Vx, Vy` and `SHR Vx, Vy`: Shift `Vx` in place, ignore `Vy`.

### Debug mode

Debug mode can be enabled via the `-d` command-line argument or by pressing P at
any time during execution. It can also be disabled at any time by pressing M.

The following commands are supported in debug mode:

* `break [ADDRESS]`: Add breakpoint to `PC` or `ADDRESS`, if given.
* `rmbreak [ADDRESS]`: Remove breakpoint at `PC` or `ADDRESS`, if given and
  exists.
* `continue`: Exit debug mode until next breakpoint or until execution is
  complete.
* `help`: Print a help string.
* `load PATH`: Load program state from `PATH`.
* `loadflags PATH`: Load flag registers from `PATH`.
* `next`: Step to the next instruction.
* `print [ATTRIBUTE]`: Print current value of the given attribute.
* `quit`: Terminate the program.
* `save PATH`: Save program state to `PATH`.
* `saveflags PATH`: Save flag registers to `PATH`.
* `set ATTRIBUTE VALUE`: Set the given attribute to the given value.

Attributes:

* `PC`: Program counter
* `SP`: Stack pointer
* `DT`: Delay timer
* `ST`: Sound timer
* `I`:  I value
* `K`:  Register to store next keypress
* `V[x]`:  All V register values or value of Vx, if given
* `R[x]`:  All R (flag) register values or value of Rx, if given
* `bfont`: Big font name
* `sfont`: Small font name
* `stack`: All stack values
* `bg`: Background color
* `fg`: Foreground color
* `$[address]`: Value at given address

If no argument is given to `print`, it will print all of the above attributes
except for address values.

## Assembler

This is an assembler for the CHIP-8 and SCHIP, utilizing libc8.

### Usage

```
c8as [-vV] [-o outputfile] src
```

* `-o` sets an output file (default is `a.c8`).
* `-v` prints diagnostic messages and the resulting hex-encoded bytecode to standard output.
* `-V` prints the version number.

Commas are optional for instruction parameters. This assembler is case
insensitive. See `examples/c8as` for example code.

## Disassembler

This is a disassembler for the CHIP-8 and SCHIP, utilizing libc8.

### Usage

```
c8dis [-al] [-o outputfile] rom
```

* `-a` toggles printing of addresses.
* `-l` toggles printing of auto-generated labels.
* `-o` writes the output to the specified file.
* `-V` prints the version number.

By default, `c8dis` will write to `stdout`.

## Testing

Build [Unity](https://github.com/ThrowTheSwitch/Unity), then `make test`.

## Further Reading

* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [S-CHIP 1.1 Reference](http://devernay.free.fr/hacks/chip8/schip.txt)
* [Octo](https://github.com/JohnEarnest/Octo)
* [CHIP-8 Extensions and Compatibility](https://chip-8.github.io/extensions/)
