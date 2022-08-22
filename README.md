# Zandra ULTRACHIP

Virtual machine based on CHIP-8, with the following improvements:

* Legacy stuff removed (ETI-660 compatibility, reserved memory for interpreter,
and SYS instruction)
* 16-bit memory addressing, with 64KiB program/data space
* 64x64 display using a 16 color 16-bit RGB color palette.
* Improved sound system (maybe?)

## Display

The display is a 16 color 64x64 display using a 16-bit RGB color palette,
organized like so:

+-----------------+
|(0,0)      (64,0)|
|                 |
|                 |
|                 |
|                 |
|                 |
|(0,64)    (64,64)|
+-----------------+

## Memory map

+-----------------+ 0xFFFF (65535) End of RAM
|                 |
|                 |
|                 |
|                 |
|                 |
| 0x000 to 0xFFFF |
|    ULTRACHIP    |
| Program / Data  |
|     Space       |
|                 |
|                 |
|                 |
+-----------------+ 0x000 (0) Start of RAM

## Registers

* V0-VF: General purpose
* PC: Program counter
* SP: Stack pointer
* C: Carry flag
* TODO Timer and Sound registers

## Instruction set

* 00E0: CLS: clear screen
* 00EE: RET: return from subroutine
* 1nnn: JP *addr*: PC=nnn
* 2nnn: CALL *addr*: SP++; S[SP]=PC; PC=nnn
* 3xkk: SE Vx, *byte*: Skip next instruction if Vx=kk
* 4xkk: SNE Vx, *byte*: Skip next instruction if Vx!=kk
* 5xy0: SE Vx, Vy: Skip next instruction if Vx=Vy
* 6xkk: LD Vx, *byte*: Vx=kk
* 7xkk: ADD Vx, *byte*: Vx=Vx+kk
* 8xy0: LD Vx, Vy: Vx=Vy
* 8xy1: OR Vx, Vy: Vx=Vx OR Vy
* 8xy2: AND Vx, Vy: Vx=Vx AND Vy
* 8xy3: XOR Vx, Vy: Vx=Vx XOR Vy
* 8xy4: ADD Vx, Vy: Vx=Vx + Vy; If (Vx > 255) C=1, else 0. Store lower 8 bits
* 8xy5: SUB Vx, Vy: Vx=Vx - Vy; If (Vx > Vy) VF=1, else 0
* TODO finish

## Sound

TODO

## Sprites

TODO
