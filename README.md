# CHIP-8 Virtual Machine and Toolkit

This is a virtual machine and disassembler for the CHIP-8 written in C,
utilizing the SDL2 library for graphics.

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
  current memory, and step through instructions individually (not yet
  implemented).
* `-v` enables verbose mode. This will print each instruction that is executed.
* `-c` sets the number of instructions to be executed per second.

Keyboard layout is the following:

```
1 2 3 4
q w e r
a s d f
z x c v
```

## Disassembler

The disassembler converts a ROM's bytecode into human-readable "assembly"
instructions.

### Usage

```
dis [-al] [-o outputfile] rom
```

* `-a` toggles printing of addresses
* `-l` toggles printing of auto-generated labels
* `-o` writes the output to the specified file