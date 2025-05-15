# CHIP-8 Virtual Machine and Toolkit

This is a virtual machine for the CHIP-8 written in C, utilizing the SDL2
library for graphics.

## Features

* Support for all vanilla CHIP-8 instructions
* Command line debugger
* Disassembler

## Installation

To install the CHIP-8 VM, run the following:

```
make
sudo make install
```

## Usage

### CHIP-8 VM

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


### Disassembler

```
dis [-al] [-o outputfile] rom
```

The disassembler will print the corresponding CHIP-8 "assembly" instructions for the ROM's bytecode. The `-a` argument toggles printing of addresses and the `-l` instruction toggles printing of labels.