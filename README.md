# CHIP-8 Toolkit

This is a virtual machine, assembler, and disassembler for the CHIP-8 written
in C, utilizing the SDL2 library for graphics.

## Installation

Make sure you have the following dependencies installed:

* libsdl2

```
make
sudo make install
```

## CHIP-8 VM

This is a fully functional CHIP-8 and Super Chip-48 virtual machine with an
integrated debug mode.

### Usage

```
c8 [-dvV] [-c clockspeed] [-p file] rom
```

* `-c` sets the number of instructions to be executed per second.
* `-d` enables debug mode. This can be used to add breakpoints, display the
  current memory, and step through instructions individually.
* `-p` loads a color palette from a file containing two newline-separated 24-bit hex codes.
* `-v` enables verbose mode. This will print each instruction that is executed.
* `-V` prints the version number.

Keyboard layout is the following:

```
1 2 3 4
q w e r
a s d f
z x c v
```

### Debug mode

Debug mode can be enabled via the `-d` command-line argument or by pressing P at
any time during execution.

The following commands are supported in debug mode:

* `break [ADDRESS]`: Add breakpoint to `PC` or `ADDRESS`, if given.
* `rmbreak [ADDRESS]`: Remove breakpoint at `PC` or `ADDRESS`, if given and
  exists.
* `continue`: Exit debug mode until next breakpoint or until execution is
  complete.
* `help`: Print a help string.
* `load PATH`: Load program state from `PATH`.
* `next`: Step to the next instruction.
* `print [ATTRIBUTE]`: Print current value of the given attribute.
* `quit`: Terminate the program.
* `save PATH`: Save program state to `PATH`.
* `set ATTRIBUTE VALUE`: Set the given attribute to the given value.

Attributes:

* `PC`: Program counter
* `SP`: Stack pointer
* `DT`: Delay timer
* `ST`: Sound timer
* `I`:  I address
* `K`:  Register to store next keypress
* `V[x]`:  All register values or value of Vx, if given
* `stack`: All stack values
* `$[address]`: Value at given address

If no argument is given to `print`, it will print all of the above attributes
except for address values.

## Assembler

The assembler converts CHIP-8 and Super Chip-48 assembly language into bytecode.

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

The disassembler converts a ROM's bytecode into human-readable assembly
instructions.

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

```
make test
make test-c8as
make test-util
```

Tests are run using [Unity](https://github.com/ThrowTheSwitch/Unity).


## Further Reading

* [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [S-CHIP 1.1 Reference](http://devernay.free.fr/hacks/chip8/schip.txt)
