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

This is a fully functional CHIP-8 virtual machine with some extra features.

### Usage

```
c8 [-dv] [-c clockspeed] rom
```

* `-d` enables debug mode. This can be used to add breakpoints, display the
  current memory, and step through instructions individually.
* `-v` enables verbose mode. This will print each instruction that is executed.
* `-c` sets the number of instructions to be executed per second.

Keyboard layout is the following:

```
1 2 3 4
q w e r
a s d f
z x c v
```

### Debug mode

The following commands are supported in debug mode:

* `break [addr]`: add breakpoint to PC or addr, if given
* `rmbreak [addr]`: Remove breakpoint at PC or addr, if given and exists
* `continue`: Exit debug mode until next breakpoint or until execution is
  complete
* `next`: Step to the next instruction
* `set [attribute] [value]`: Set the given attribute to the given value
* `print [attribute]`: Print current value of given attribute
* `help`: Print a help string
* `quit`: Terminate the program

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

The assembler converts CHIP-8 assembly language into bytecode.

### Usage

```
c8as [-v] [-o outputfile] src
```

* `-v` prints the resulting hex-formatted bytecode to standard output
* `-o` sets an output file (default is `a.c8`)

Commas are optional for instruction parameters. See `tests/c8as` for example code.

## Disassembler

The disassembler converts a ROM's bytecode into human-readable assembly
instructions.

### Usage

```
c8dis [-al] [-o outputfile] rom
```

* `-a` toggles printing of addresses
* `-l` toggles printing of auto-generated labels
* `-o` writes the output to the specified file

By default, `dis` will write to `stdout`.