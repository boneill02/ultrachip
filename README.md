# CHIP-8 Virtual Machine and Toolkit

This is a virtual machine for the CHIP-8 written in C, utilizing the SDL2 library for graphics.

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
chip8 [-d] rom
```

Debug mode allows stepping and printing of the current instruction. At any point during execution, debug mode can be enabled or disabled with the P and M keys, respectively. P parses the current instruction and prints the next instruction.

### Disassembler

```
dis [-al] [-o outputfile] rom
```

The disassembler will print the corresponding CHIP-8 "assembly" instructions for the ROM's bytecode. The `-a` argument toggles printing of addresses and the `-l` instruction toggles printing of labels.